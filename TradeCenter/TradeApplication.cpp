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
	// 初始化日志环境
	g_preLogStr.append(__FUNCTION__);
	g_preLogStr.append(" Running------------");
	g_pszAppConfigFile = configFilePath;
	g_sconnected = ConnectStatus::NotConnect;
	//载入配置
	setConfig();
	InitializeCriticalSection(&g_cs_s);
	InitializeCriticalSection(&g_cs_r);
	g_sThread = NULL;
	g_rThread = NULL;
}

void NQ::TradeApplication::setConfig(){
	try{
		// 复用quickfix的设置类解析ini配置文件
		FIX::SessionSettings appSetting(g_pszAppConfigFile);

		const FIX::Dictionary& AppDict = appSetting.get();   // 取默认节[DEFAULT]配置
		g_strPublicKey = AppDict.getString("PublicKey");   // 券商分配的私钥
		std::string encryptTypeStr = AppDict.getString("EncryptType");
		g_strEncryptType = EncryptMethod(atoi(encryptTypeStr.c_str())); // 加密方式

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
		// 将配置信息打印出来
		g_preLogStr.append("\n\t\t\t加密方式: ");
		g_preLogStr.append(encryptTypeStr);
		g_preLogStr.append("\n\t\t\t账户类型: ");
		g_preLogStr.append(inputType);
		g_preLogStr.append("\n\t\t\t资金帐号: ");
		g_preLogStr.append(g_user.accountNo);
		g_preLogStr.append("\n\t\t\t密码:     ");
		g_preLogStr.append(g_user.password);
		g_preLogStr.append("\n\t\t\t私钥:     ");
		g_preLogStr.append(g_strPublicKey);
	}
	catch(std::exception& Exp){
		// 忽略所有异常，取值失败时取默认值
		std::cout << g_preLogStr << std::endl;
		std::cout << "读取配置异常：" << Exp.what() << std::endl;
	}
}


NQ::TradeApplication::~TradeApplication(void)
{
	//delete(g_sessionId);
}

void NQ::TradeApplication::onCreate(const FIX::SessionID &sessionID){
	// 连接中
	g_sconnected = ConnectStatus::Connecting;
	initSessionLog(sessionID);

	g_log->onEvent(__FUNCTION__);
	g_log->onEvent("    SessionID: " + sessionID.toStringFrozen());
}


void NQ::TradeApplication::onLogon( const FIX::SessionID &sessionID )
{
	//已连接
	g_sconnected = ConnectStatus::Connected;
	initSessionLog(sessionID);
}

void NQ::TradeApplication::onLogout( const FIX::SessionID &sessionID )
{
	//断开连接
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
		// 需要多线程操作吗？
		// 返回报单回报赋值
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
		// 如果不存在与回报对应的请求
		// 如何处理
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
		//g_log->onEvent("发送退出消息" + msg.toString());
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

// 是否配对请求
bool NQ::TradeApplication::isRespCorresponant(const FIX::Message &message)
{
	FIX::ExecTransType execTransType;
	// 如果应答发送类别为3，则为委托状态查询应答，clOrdId无法对应，直接查询请求列表中是否有MsgType=H的发报即可
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
	// 如果消息类型为UAP，则查询是否有UAN的发送请求，并且两者PosReqID必须一致
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
				//如果是唯一一条或者多条的最后一条，则删除请求列表中该请求
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
	// 其余回报，根据clOrdId来对应请求，如果存在则对应
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

// 设置登录参数，用户名密码等
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
		g_log->onEvent("不加密的密文: " + std::string(szPasswordRaw));
	}
	else if(g_strEncryptType == EncryptMethod::DesEcb){
		//DES-ECB加密密码
		int iSize = Encrypt_org( szPasswordRaw, g_user.password.c_str(), g_strPublicKey.c_str(), encryptType );
		g_log->onEvent("加密后的密文: " + std::string(szPasswordRaw, iSize));
		//按约定格式序列化密文
		SerializeData(szPasswordRaw, iSize, szPasswordRawS, iSize*2);
		g_log->onEvent("序列化的密文: " + std::string(szPasswordRawS, iSize*2));
	}
	else{
		//BlowFish加密密码
	}
	// 按约定格式组合验证字段
	_snprintf(szPsdValue, sizeof(szPsdValue)-1, "%c:%s:%s:", 
		g_user.type, g_user.accountNo.c_str(), szPasswordRawS);
	// 将验证文本写入登录消息中
	message.setField(FIX::FIELD::RawData, szPsdValue);
	// 将加密类型写入登录消息中，告知服务端密码的加密方式
	message.setField(FIX::FIELD::EncryptMethod, encryptType);

	g_log->onEvent("发送登录消息: ");
	g_log->onEvent(message.toString());
}

//发送限价报单请求
void NQ::TradeApplication::sendLimitedOrder(LimitedOrder& order) {

	g_log->onEvent(__FUNCTION__);

	//配置message节点值
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

	//配置message节点值
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

	//配置message节点值
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

	//配置message节点值
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
	//依次发送队列里的请求
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
		//全集报告合并
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

// 设置回调类对象
void NQ::TradeApplication::setCaller(NQ::TradeCenter* tradeCenter)
{
	this->c_tradeCenter = tradeCenter;
}

// 根据报文返回请求类型
NQ::ReqType NQ::TradeApplication::getReqType(const FIX::Message &message) 
	//throw(NQ::MessageIncorrectError)
{
	FIX::MsgType msgType;
	if (!message.getHeader().getFieldIfSet(msgType))
	{
		throw NQ::LackOfFieldError("MsgType 35");
	}
	// 单笔委托，消息类型为D
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
	// 撤单请求，消息类型为F
	if (msgType == FIX::MsgType_OrderCancelRequest)
	{
		return ReqType::CancelOrderType;
	}
	if (msgType == FIX::MsgType_OrderStatusRequest)
	{
		return ReqType::OrderQueryType;
	}
	// 资金股份查询，消息类型为UAN
	if (msgType == NQ::MsgType_UAN)
	{
		FIX::PosReqType qryType;
		if (!message.getFieldIfSet(qryType))
		{
			throw NQ::LackOfFieldError("PosReqType 724");
		}
		// 持仓查询
		if (qryType == FIX::PosReqType_POSITIONS)
		{
			return ReqType::StockQueryType;
		}
		// 资金查询
		if (qryType == NQ::PosReqType_FUNDS){
			return ReqType::FundQueryType;
		}
	}
	return ReqType::DefaultReq;
}

// 根据报文查询回报的类型
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
		// 持仓查询回报
		if (qryType == FIX::PosReqType_POSITIONS)
		{
			return RespType::StockQueryResultType;
		}
		// 资金查询回报
		if (qryType == NQ::PosReqType_FUNDS)
		{
			return RespType::FundQueryResultType;
		}
	}
	FIX::ExecTransType execTransType;
	if (message.getFieldIfSet(execTransType))
	{
		// 应答发送类别为3时，消息为报单状态查询结果
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
		// 委托状态为0时，消息为委托确认消息
		if (orderStatus == FIX::OrdStatus_NEW)
		{
			return RespType::OrderConfirmType;
		}
		if (orderStatus == FIX::OrdStatus_PENDING_NEW)
		{
			return RespType::OrderApplyingType;
		}
		// 委托状态为8时，消息为委托拒绝
		if (orderStatus == FIX::OrdStatus_REJECTED)
		{
			return RespType::OrderRejectType;
		}
		// 委托状态为1或2时，消息为执行回报
		if (orderStatus == FIX::OrdStatus_PARTIALLY_FILLED 
			|| orderStatus == FIX::OrdStatus_FILLED)
		{
			return RespType::OrderExecuteType;
		}
		// 委托状态为6时，消息为撤单确认或执行回报
		if (orderStatus == FIX::OrdStatus_PENDING_CANCEL)
		{
			FIX::ExecType execType;
			if (!message.getFieldIfSet(execType))
			{
				throw NQ::LackOfFieldError("ExecType 150");
			}
			// 执行结果为6时，消息为撤单确认
			if (execType == FIX::ExecType_PENDING_CANCEL)
			{
				return RespType::CancelCommitType;
			}
			// 执行结果为1或2时，消息为执行回报
			if (execType == FIX::ExecType_PARTIAL_FILL || execType == FIX::ExecType_FILL)
			{
				return RespType::OrderExecuteType;
			}
		}
		// 委托状态为4时，消息为撤单回报
		if (orderStatus == FIX::OrdStatus_CANCELED)
		{
			return RespType::CancelExecuteType;
		}
	}
	// 消息类型为9时，消息为撤单拒绝
	if (msgType == FIX::MsgType_OrderCancelReject)
	{
		return RespType::CancelRejectType;
	}
	// 消息类型为UAP时，消息为资金股份查询回报
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