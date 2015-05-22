#include "TradeApplication.h"
#include "TradeCenter.h"
#include "TradeError.h"


NQ::TradeApplication::TradeApplication(void)
{
	DeleteCriticalSection(&g_cs_s);
	DeleteCriticalSection(&g_cs_r);
}

NQ::TradeApplication::TradeApplication(
	const FilePath& configFilePath)
{
	// ��ʼ����־����
	g_preLogStr.append(__FUNCTION__);
	g_preLogStr.append(" Running------------");
	g_pszAppConfigFile = configFilePath;
	g_sconnected = ConnectStatus::NotConnect;
	//��������
	setConfig();
	InitializeCriticalSection(&g_cs_s);
	InitializeCriticalSection(&g_cs_r);
	g_sThread = NULL;
	g_rThread = NULL;
}

void NQ::TradeApplication::setConfig(){
	try{
		// ����quickfix�����������ini�����ļ�
		FIX::SessionSettings appSetting(g_pszAppConfigFile);

		const FIX::Dictionary& AppDict = appSetting.get();   // ȡĬ�Ͻ�[DEFAULT]����
		g_strPublicKey = AppDict.getString("PublicKey");   // ȯ�̷����˽Կ
		std::string encryptTypeStr = AppDict.getString("EncryptType");
		g_strEncryptType = EncryptMethod(atoi(encryptTypeStr.c_str())); // ���ܷ�ʽ

		std::string inputType = AppDict.getString("InputType");
		/*
		if (inputType.c_str()[0]=='G'){
		g_user.type = AccountType::IndividualOptionsAccount;
		}else if (inputType.c_str()[0] == 'Z'){
		g_user.type = AccountType::NormalAccount;
		}else if (inputType.c_str()[0] == 'X'){
		g_user.type = AccountType::CreditAccount;
		}else {
		g_user.type = AccountType::CombinedAccount;
		}
		*/
		//20150510
		g_user.type = AccountType(inputType.c_str()[0]);
		g_user.accountNo = AppDict.getString("UserName");
		g_user.password	= AppDict.getString("Password");
		// ��������Ϣ��ӡ����
		g_preLogStr.append("\n\t\t\t���ܷ�ʽ: ");
		g_preLogStr.append(encryptTypeStr);
		g_preLogStr.append("\n\t\t\t�˻�����: ");
		g_preLogStr.append(inputType);
		g_preLogStr.append("\n\t\t\t�ʽ��ʺ�: ");
		g_preLogStr.append(g_user.accountNo);
		g_preLogStr.append("\n\t\t\t����:     ");
		g_preLogStr.append(g_user.password);
		g_preLogStr.append("\n\t\t\t˽Կ:     ");
		g_preLogStr.append(g_strPublicKey);
	}
	catch(std::exception& Exp){
		// ���������쳣��ȡֵʧ��ʱȡĬ��ֵ
		std::cout << g_preLogStr << std::endl;
		std::cout << "��ȡ�����쳣��" << Exp.what() << std::endl;
	}
}


NQ::TradeApplication::~TradeApplication(void)
{
	//delete(g_sessionId);
}

void NQ::TradeApplication::onCreate(const FIX::SessionID &sessionID){
	// ������
	g_sconnected = ConnectStatus::Connecting;
	initSessionLog(sessionID);

	g_log->onEvent(__FUNCTION__);
	g_log->onEvent("    SessionID: " + sessionID.toStringFrozen());
}


void NQ::TradeApplication::onLogon( const FIX::SessionID &sessionID )
{
	//������
	g_sconnected = ConnectStatus::Connected;
	initSessionLog(sessionID);
}

void NQ::TradeApplication::onLogout( const FIX::SessionID &sessionID )
{
	//�Ͽ�����
	g_sconnected = ConnectStatus::NotConnect;
	initSessionLog(sessionID);
}

void NQ::TradeApplication::fromApp( const FIX::Message &message, const FIX::SessionID &sessionID )
	throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
	initSessionLog(sessionID);

	//g_log->onIncoming(__FUNCTION__);
	//g_log->onIncoming(message.toString());
	try{
		if (!isRespCorresponant(message))
		{
			throw NQ::NotCorrespondingError();
		}
		// ��Ҫ���̲߳�����
		// ���ر����ر���ֵ
		RespType respType = getRespType(message);
		switch(respType){
		case RespType::OrderApplyingType:
			break;
		case RespType::OrderConfirmType:
		case RespType::OrderRejectType:
			{
				OrderResponse orderResp = NewLimitedOrderSingle::genOrderResponse(message);
				this->g_ordResps.push_back(orderResp);
				//c_tradeCenter->callBack(orderResp, g_user.accountNo);
				break;
			}
		case RespType::OrderExecuteType:
			{
				ExecuteResponse execResp = NewLimitedOrderSingle::genExecuteResponse(message);
				this->g_execResps.push_back(execResp);
				//c_tradeCenter->callBack(execResp, g_user.accountNo);
				break;
			}
		case RespType::CancelCommitType:
		case RespType::CancelRejectType:
		case RespType::CancelExecuteType:
			{
				CancelResponse cancelResp = CancelRequest::genCancelResponse(message);
				this->g_cancResps.push_back(cancelResp);
				//c_tradeCenter->callBack(cancelResp, g_user.accountNo);
				break;
			}
		case RespType::OrderQueryResultType:
			{
				OrderQueryResponse ordQryResp = OrderQueryRequest::genOrderQueryResponse(message);
				this->g_ordQryResps.push_back(ordQryResp);
				//c_tradeCenter->callBack(ordQryResult, g_user.accountNo);
				break;
			}
		case RespType::FundQueryResultType:
			{
				FundPosQueryResponse fundPosQryResp = FundPositionsQueryRequest::genFundPosQryResponse(message);
				this->g_fundPosQryResps.push_back(fundPosQryResp);
				if (FundPositionsQueryRequest::isSingleReport(message)
					|| FundPositionsQueryRequest::isLastReport(message))
				{
					CLock cl(&g_cs_r);
					if (!isRecvThreadRunning() ){
						g_rThread = new std::thread(&TradeApplication::receveThread, this);
					}
				}
				return;
				break;
			}
		default:
			break;
		}
		CLock cl(&g_cs_r);
		if (!isRecvThreadRunning() ){
			g_rThread = new std::thread(&TradeApplication::receveThread, this);
		}
	}catch(NQ::NotCorrespondingError e){
		// �����������ر���Ӧ������
		// ��δ���
		return;
	}
}

void NQ::TradeApplication::toApp( FIX::Message &message, const FIX::SessionID &sessionID )
	throw( FIX::DoNotSend )
{
	initSessionLog(sessionID);
	//g_log->onOutgoing(__FUNCTION__ );
	//g_log->onOutgoing(message.toString());
}

void NQ::TradeApplication::toAdmin( FIX::Message &msg, const FIX::SessionID &sessionID){
	//initSessionLog(sessionID);

	//g_log->onOutgoing(__FUNCTION__ + msg.toString());
	//g_log->onEvent(__FUNCTION__);
	//g_log->onEvent("    SessionID: " + sessionID.toStringFrozen());

	FIX::MsgType msgType;
	msg.getHeader().getField(msgType);
	if (msgType == FIX::MsgType_Logon)
	{
		setLogonConfig(msg);
	}
	else if(msgType == FIX::MsgType_Logout)
	{
		//g_log->onEvent("�����˳���Ϣ" + msg.toString());
	} else if(msgType == FIX::MsgType_SequenceReset)
	{
		g_log->onEvent(__FUNCTION__);
		//g_log->onEvent("    SessionID: " + sessionID.toStringFrozen());
		g_log->onEvent("    Sequence: " + msg.getField(FIX::FIELD::NewSeqNo));
	}
}


void NQ::TradeApplication::fromAdmin( const FIX::Message &msg, const FIX::SessionID & sessionID) 
	throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon )
{
	initSessionLog(sessionID);
	g_log->onEvent(__FUNCTION__);
	g_log->onEvent(msg.toString());
	if (msg.getHeader().getField(FIX::FIELD::MsgType)==FIX::MsgType_SequenceReset)
	{
		g_log->onEvent("    Sequence: " + msg.getField(FIX::FIELD::NewSeqNo));
	}
}

// �Ƿ��������
bool NQ::TradeApplication::isRespCorresponant(const FIX::Message &message)
{
	FIX::ExecTransType execTransType;
	// ���Ӧ�������Ϊ3����Ϊί��״̬��ѯӦ��clOrdId�޷���Ӧ��ֱ�Ӳ�ѯ�����б����Ƿ���MsgType=H�ķ�������
	if (message.getFieldIfSet(execTransType) 
		&& execTransType.getValue() == NQ::ExecTransType_ORDQRY )
	{
		for (Orders::iterator it = g_orders.begin(); it!= g_orders.end(); ++it)
		{
			if (it->second->getHeader().getField(FIX::FIELD::MsgType) == FIX::MsgType_OrderStatusRequest
				&& it->second->getField(FIX::FIELD::ClOrdID) == message.getField(FIX::FIELD::ClOrdID))
			{
				delete it->second;
				g_orders.erase(it);
				return true;
			}
		}
		return false;
	}
	FIX::MsgType msgType;
	message.getHeader().getFieldIfSet(msgType);
	// �����Ϣ����ΪUAP�����ѯ�Ƿ���UAN�ķ������󣬲�������PosReqID����һ��
	if (msgType.getValue() == NQ::MsgType_UAP)
	{
		FIX::PosReqID posReqId;
		if (!message.getFieldIfSet(posReqId))
		{
			throw NQ::LackOfFieldError("PosReqID 710");
		}
		for (Orders::iterator it = g_orders.begin(); it!= g_orders.end(); ++it)
		{
			if (it->second->getHeader().getField(FIX::FIELD::MsgType) == NQ::MsgType_UAN
				&& it->second->getField(FIX::FIELD::PosReqID) == posReqId)
			{
				//�����Ψһһ�����߶��������һ������ɾ�������б��и�����
				if (FundPositionsQueryRequest::isSingleReport(message)
					|| FundPositionsQueryRequest::isLastReport(message))
				{
					delete it->second;
					g_orders.erase(it);
				}
				return true;
			}
		}
		return false;
	}
	// ����ر�������clOrdId����Ӧ��������������Ӧ
	FIX::ClOrdID msgClOrdID;
	message.getFieldIfSet(msgClOrdID);
	if (g_orders.find(msgClOrdID.getValue())==g_orders.end())
	{
		return false;
	}
	FIX::Message* oriMessage = g_orders.find(msgClOrdID.getValue())->second;
	if (msgType == FIX::MsgType_ExecutionReport )
	{ 
		if (message.getField(FIX::FIELD::OrdStatus)[0] == FIX::OrdStatus_REJECTED
			|| message.getField(FIX::FIELD::LeavesQty)=="0"
			|| message.getField(FIX::FIELD::OrdStatus).c_str()[0]==FIX::OrdStatus_CANCELED)
		{
			delete oriMessage;
			g_orders.erase(msgClOrdID.getValue());
		}


	}
	return true;
}

// ���õ�¼�������û��������
void NQ::TradeApplication::setLogonConfig(FIX::Message &message){
	g_log->onEvent(__FUNCTION__);
	g_log->onEvent(message.toString());
	char szPsdValue[1024] = {0};
	char encryptType[3] = {0};
	char szPasswordRaw[128]  = {0};
	char szPasswordRawS[256]  = {0};
	itoa(g_strEncryptType, encryptType, 10);

	if(g_strEncryptType == EncryptMethod::None){
		strcpy(szPasswordRawS, g_user.password.c_str());
		g_log->onEvent("�����ܵ�����: " + std::string(szPasswordRaw));
	}
	else if(g_strEncryptType == EncryptMethod::DesEcb){
		//DES-ECB��������
		int iSize = Encrypt_org( szPasswordRaw, g_user.password.c_str(), g_strPublicKey.c_str(), encryptType );
		g_log->onEvent("���ܺ������: " + std::string(szPasswordRaw, iSize));
		//��Լ����ʽ���л�����
		SerializeData(szPasswordRaw, iSize, szPasswordRawS, iSize*2);
		g_log->onEvent("���л�������: " + std::string(szPasswordRawS, iSize*2));
	}
	else{
		//BlowFish��������
	}
	// ��Լ����ʽ�����֤�ֶ�
	_snprintf(szPsdValue, sizeof(szPsdValue)-1, "%c:%s:%s:", 
		g_user.type, g_user.accountNo.c_str(), szPasswordRawS);
	// ����֤�ı�д���¼��Ϣ��
	message.setField(FIX::FIELD::RawData, szPsdValue);
	// ����������д���¼��Ϣ�У���֪���������ļ��ܷ�ʽ
	message.setField(FIX::FIELD::EncryptMethod, encryptType);

	g_log->onEvent("���͵�¼��Ϣ: ");
	g_log->onEvent(message.toString());
}

//�����޼۱�������
void NQ::TradeApplication::sendLimitedOrder(LimitedOrder& order) {

	g_log->onEvent(__FUNCTION__);

	//����message�ڵ�ֵ
	NewLimitedOrderSingle* t_order = new NewLimitedOrderSingle(order);
	g_log->onEvent("order id: " + order.id);
	if (order.response.respCode != RespCode::SUCC)
	{
		return;
	}

	if(!isConnected()) {
		g_log->onEvent("Exception: not recently logon");
		throw NQ::ReqWhenNotLogonError();
		return;
	}

	CLock cl(&g_cs_s);
	g_reqList.push_back(t_order);
	if (!isSendThreadRunning() && !g_reqList.empty()){
		g_sThread = new std::thread(&TradeApplication::sendThread, this);
	}
}
void NQ::TradeApplication::sendCancelOrder(CancelOrder& cancel){

	g_log->onEvent(__FUNCTION__);

	//����message�ڵ�ֵ
	CancelRequest* t_cancel = new CancelRequest(cancel);
	g_log->onEvent("cancel id: " + cancel.orderId);
	if (cancel.response.respCode != RespCode::SUCC)
	{
		return;
	}

	if(!isConnected()) {
		g_log->onEvent("Exception: not recently logon");
		throw NQ::ReqWhenNotLogonError();
		return;
	}

	CLock cl(&g_cs_s);
	g_reqList.push_back(t_cancel);
	if (!isSendThreadRunning() && !g_reqList.empty()){
		g_sThread = new std::thread(&TradeApplication::sendThread, this);
	}
}
void NQ::TradeApplication::sendQueryOrder(OrderQuery& query){

	g_log->onEvent(__FUNCTION__);

	//����message�ڵ�ֵ
	OrderQueryRequest* t_query = new OrderQueryRequest(query);
	g_log->onEvent("query id: " + query.orderId);
	if (query.response.respCode != RespCode::SUCC)
	{
		return;
	}

	if(!isConnected()) {
		g_log->onEvent("Exception: not recently logon");
		throw NQ::ReqWhenNotLogonError();
		return;
	}

	CLock cl(&g_cs_s);
	g_reqList.push_back(t_query);
	if (!isSendThreadRunning() && !g_reqList.empty()){
		g_sThread = new std::thread(&TradeApplication::sendThread, this);
	}
}
void NQ::TradeApplication::sendFundPosQry(FundPosQuery& query){

	g_log->onEvent(__FUNCTION__);

	//����message�ڵ�ֵ
	FundPositionsQueryRequest* t_query = new FundPositionsQueryRequest(query);
	g_log->onEvent("query id: " + query.id);
	if (query.response.respCode != RespCode::SUCC)
	{
		return;
	}

	if(!isConnected()) {
		g_log->onEvent("Exception: not recently logon");
		throw NQ::ReqWhenNotLogonError();
		return;
	}

	CLock cl(&g_cs_s);
	g_reqList.push_back(t_query);
	if (!isSendThreadRunning() && !g_reqList.empty()){
		g_sThread = new std::thread(&TradeApplication::sendThread, this);
	}
}

void NQ::TradeApplication::sendThread(){
	//���η��Ͷ����������
	bool ret = false;
	while (!g_reqList.empty() && isConnected())
	{
		FIX::Message* t_message = g_reqList.front();
		g_log->onEvent("    g_sessionId: " + g_sessionId.toStringFrozen());
		//std::cout<<t_message->getField(FIX::FIELD::ClOrdID)<<std::endl;
		ret = FIX::Session::sendToTarget(*t_message, g_sessionId);

		if (ret == true)
		{
			CLock cl(&g_cs_s);
			//delete t_message;
			if (t_message->getHeader().getField(FIX::FIELD::MsgType) == NQ::MsgType_UAN)
			{
				g_orders.insert(Orders::value_type(t_message->getField(FIX::FIELD::PosReqID), t_message));
			} else {
				g_orders.insert(Orders::value_type(t_message->getField(FIX::FIELD::ClOrdID), t_message));
			}

			g_reqList.pop_front();
		}
	}
	g_sThread = NULL;
	//return 0;
}

void NQ::TradeApplication::receveThread(){
	while (!g_ordResps.empty() && c_tradeCenter)
	{
		CLock cl(&g_cs_r);
		OrderResponse ordResp = g_ordResps.front();
		g_log->onEvent("Call back" + ordResp.id);
		c_tradeCenter->callBack(ordResp, g_user.accountNo);
		g_ordResps.pop_front();
	}
	while (!g_execResps.empty() && c_tradeCenter)
	{
		CLock cl(&g_cs_r);
		ExecuteResponse execResp = g_execResps.front();
		g_log->onEvent("Call back" + execResp.id);
		c_tradeCenter->callBack(execResp, g_user.accountNo);
		g_execResps.pop_front();
	}
	while (!g_cancResps.empty() && c_tradeCenter)
	{
		CLock cl(&g_cs_r);
		CancelResponse cancResp = g_cancResps.front();
		g_log->onEvent("Call back" + cancResp.orderId);
		c_tradeCenter->callBack(cancResp, g_user.accountNo);
		g_cancResps.pop_front();
	}
	while (!g_ordQryResps.empty() && c_tradeCenter)
	{
		CLock cl(&g_cs_r);
		OrderQueryResponse ordQryResp = g_ordQryResps.front();
		g_log->onEvent("Call back" + ordQryResp.orderId);
		c_tradeCenter->callBack(ordQryResp, g_user.accountNo);
		g_ordQryResps.pop_front();
	}
	while (!g_fundPosQryResps.empty() && c_tradeCenter)
	{
		CLock cl(&g_cs_r);
		//ȫ������ϲ�
		FundPosQueryResponse fundPosQryResp = g_fundPosQryResps.back();
		g_fundPosQryResps.pop_back();
		FundPositionsQueryRequest::mergeRespnse(fundPosQryResp, this->g_fundPosQryResps);

		g_log->onEvent("Call back" + fundPosQryResp.id);
		c_tradeCenter->callBack(fundPosQryResp, g_user.accountNo);
		//g_fundPosQryResps.pop_front();
	}
	g_rThread = NULL;
}


bool NQ::TradeApplication::isConnected(){
	return this->g_sconnected == NQ::ConnectStatus::Connected;
}

bool NQ::TradeApplication::isConnecting(){
	return this->g_sconnected == NQ::ConnectStatus::Connecting;
}

bool NQ::TradeApplication:: isSendThreadRunning(){
	return NULL != this->g_sThread;
}

bool NQ::TradeApplication:: isRecvThreadRunning(){
	return NULL != this->g_rThread;
}

NQ::UserConfiguration* NQ::TradeApplication::getUserConfig(){
	return &(this->g_user);
}

// ���ûص������
void NQ::TradeApplication::setCaller(NQ::TradeCenter* tradeCenter)
{
	this->c_tradeCenter = tradeCenter;
}

// ���ݱ��ķ�����������
NQ::ReqType NQ::TradeApplication::getReqType(const FIX::Message &message) 
	//throw(NQ::MessageIncorrectError)
{
	FIX::MsgType msgType;
	if (!message.getHeader().getFieldIfSet(msgType))
	{
		throw NQ::LackOfFieldError("MsgType 35");
	}
	// ����ί�У���Ϣ����ΪD
	if (msgType == FIX::MsgType_NewOrderSingle)
	{
		FIX::OrdType orderType;
		if (!message.getFieldIfSet(orderType))
		{
			throw NQ::LackOfFieldError("OrdType 40");
		}
		if (orderType == FIX::OrdType_LIMIT)
		{
			return ReqType::LimitedOrderType;
		}
	}
	// ����������Ϣ����ΪF
	if (msgType == FIX::MsgType_OrderCancelRequest)
	{
		return ReqType::CancelOrderType;
	}
	if (msgType == FIX::MsgType_OrderStatusRequest)
	{
		return ReqType::OrderQueryType;
	}
	// �ʽ�ɷݲ�ѯ����Ϣ����ΪUAN
	if (msgType == NQ::MsgType_UAN)
	{
		FIX::PosReqType qryType;
		if (!message.getFieldIfSet(qryType))
		{
			throw NQ::LackOfFieldError("PosReqType 724");
		}
		// �ֲֲ�ѯ
		if (qryType == FIX::PosReqType_POSITIONS)
		{
			return ReqType::StockQueryType;
		}
		// �ʽ��ѯ
		if (qryType == NQ::PosReqType_FUNDS){
			return ReqType::FundQueryType;
		}
	}
	return ReqType::DefaultReq;
}

// ���ݱ��Ĳ�ѯ�ر�������
NQ::RespType NQ::TradeApplication::getRespType(const FIX::Message &message) 
	//throw(NQ::MessageIncorrectError)
{
	FIX::MsgType msgType;
	if (!message.getHeader().getFieldIfSet(msgType))
	{
		throw NQ::LackOfFieldError("MsgType 35");
	}
	if (msgType == NQ::MsgType_UAP)
	{
		FIX::PosReqType qryType;
		if (!message.getFieldIfSet(qryType))
		{
			throw NQ::LackOfFieldError("PosReqType 724");
		}
		// �ֲֲ�ѯ�ر�
		if (qryType == FIX::PosReqType_POSITIONS)
		{
			return RespType::StockQueryResultType;
		}
		// �ʽ��ѯ�ر�
		if (qryType == NQ::PosReqType_FUNDS)
		{
			return RespType::FundQueryResultType;
		}
	}
	FIX::ExecTransType execTransType;
	if (message.getFieldIfSet(execTransType))
	{
		// Ӧ�������Ϊ3ʱ����ϢΪ����״̬��ѯ���
		if (execTransType == FIX::ExecTransType_STATUS)
		{
			return RespType::OrderQueryResultType;
		}
	}
	if (msgType == FIX::MsgType_ExecutionReport)
	{
		FIX::OrdStatus orderStatus;
		if (!message.getFieldIfSet(orderStatus))
		{
			throw NQ::LackOfFieldError("OrdStatus 39");
		}
		// ί��״̬Ϊ0ʱ����ϢΪί��ȷ����Ϣ
		if (orderStatus == FIX::OrdStatus_NEW)
		{
			return RespType::OrderConfirmType;
		}
		if (orderStatus == FIX::OrdStatus_PENDING_NEW)
		{
			return RespType::OrderApplyingType;
		}
		// ί��״̬Ϊ8ʱ����ϢΪί�оܾ�
		if (orderStatus == FIX::OrdStatus_REJECTED)
		{
			return RespType::OrderRejectType;
		}
		// ί��״̬Ϊ1��2ʱ����ϢΪִ�лر�
		if (orderStatus == FIX::OrdStatus_PARTIALLY_FILLED 
			|| orderStatus == FIX::OrdStatus_FILLED)
		{
			return RespType::OrderExecuteType;
		}
		// ί��״̬Ϊ6ʱ����ϢΪ����ȷ�ϻ�ִ�лر�
		if (orderStatus == FIX::OrdStatus_PENDING_CANCEL)
		{
			FIX::ExecType execType;
			if (!message.getFieldIfSet(execType))
			{
				throw NQ::LackOfFieldError("ExecType 150");
			}
			// ִ�н��Ϊ6ʱ����ϢΪ����ȷ��
			if (execType == FIX::ExecType_PENDING_CANCEL)
			{
				return RespType::CancelCommitType;
			}
			// ִ�н��Ϊ1��2ʱ����ϢΪִ�лر�
			if (execType == FIX::ExecType_PARTIAL_FILL || execType == FIX::ExecType_FILL)
			{
				return RespType::OrderExecuteType;
			}
		}
		// ί��״̬Ϊ4ʱ����ϢΪ�����ر�
		if (orderStatus == FIX::OrdStatus_CANCELED)
		{
			return RespType::CancelExecuteType;
		}
	}
	// ��Ϣ����Ϊ9ʱ����ϢΪ�����ܾ�
	if (msgType == FIX::MsgType_OrderCancelReject)
	{
		return RespType::CancelRejectType;
	}
	// ��Ϣ����ΪUAPʱ����ϢΪ�ʽ�ɷݲ�ѯ�ر�
	return RespType::DefaultResp;
}


NQ::TradeRespBase NQ::TradeApplication::setResponse(RespCode respCode, TradeResult errorMessage){
	return TradeRespBase(respCode, errorMessage);
}

void NQ::TradeApplication::initSessionLog(const FIX::SessionID &sessionID ){
	//if(0==strcmp(g_sessionId.getBeginString().getValue().c_str(),"")){
	if (g_sessionId.getBeginString().getValue().empty()
		|| g_sessionId != sessionID){
			g_sessionId = sessionID;
	}
	if(!g_log){
		FIX::Session* session = FIX::Session::lookupSession( g_sessionId );
		g_log = session->getLog();
		g_log->onEvent(g_preLogStr);
		g_log->onEvent(__FUNCTION__);
		g_log->onEvent("    SessionID: " + g_sessionId.toStringFrozen());
	}
}