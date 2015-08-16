#include "TradeCenter.h"


void NQ::TradeCenter::releaseCallers()
{
	for (Callers::iterator it=callers.begin();it!=callers.end();++it)
	{
		delete (*it).second;
	}
	callers.clear();
}

// ��ʼ������
AccountNo NQ::TradeCenter::init(const char* pszAppConfigFile, const char* pszEngineConfigFile, IOrderCaller* caller){
	FIX::SessionSettings* settings = new FIX::SessionSettings( pszEngineConfigFile );

	NQ::TradeApplication* application = new NQ::TradeApplication(pszAppConfigFile);
	application->setCaller(this);
	FIX::FileStoreFactory* storeFactory = new FIX::FileStoreFactory(*settings );  // �ļ���ʽ�洢��Ϣ
	FIX::FileLogFactory* logFactory = new FIX::FileLogFactory(*settings );      // �ļ���ʽ�洢��־
	FIX::SocketInitiator* initiator = new FIX::SocketInitiator(*application, *storeFactory, *settings, *logFactory );

	AccountNo user = application->getUserConfig()->accountNo;
	if (callers.find(user) == callers.end())
	{
		callers.insert(
			Callers::value_type(user, 
			new AppConfiguration(application,storeFactory,logFactory,settings,initiator)));
	}

	this->orderCaller = caller;
	return user;
}

// ��������
void NQ::TradeCenter::start(AccountNo accountNo){
	findInitiator(accountNo)->start();
}

// ֹͣ����
void NQ::TradeCenter::stop(AccountNo accountNo){
	findInitiator(accountNo)->stop();
}

// �����޼��µ�
void NQ::TradeCenter::reqLimitedOrder(LimitedOrder& order, AccountNo user){
	//if (application.isConnected()) 
	try{
		findApplication(user)->sendLimitedOrder(order);
		order.response = TradeApplication::setResponse(RespCode::SUCC,"");
	}catch(NQ::ReqWhenNotLogonError){
		order.response = TradeApplication::setResponse(RespCode::FAIL, "�˻���δ��¼���޷�ִ�н���: ");
	}
}

// ���󳷵� 
void NQ::TradeCenter::reqCancelOrder(CancelOrder& cancel, AccountNo user)
{
	try{
		findApplication(user)->sendCancelOrder(cancel);
		cancel.response = TradeApplication::setResponse(RespCode::SUCC,"");
	}catch(NQ::ReqWhenNotLogonError){
		cancel.response = TradeApplication::setResponse(RespCode::FAIL, "�˻���δ��¼���޷�ִ�н���: ");
	}
}

// ����ֲֲ�ѯ
void NQ::TradeCenter::reqOrderQuery(OrderQuery& query, AccountNo user)
{
	try{
		findApplication(user)->sendQueryOrder(query);
		query.response = TradeApplication::setResponse(RespCode::SUCC,"");
	}catch(NQ::ReqWhenNotLogonError){
		query.response = TradeApplication::setResponse(RespCode::FAIL, "�˻���δ��¼���޷�ִ�н���: ");
	}
}

//�����ʽ�ɷݲ�ѯ
void NQ::TradeCenter::reqFundPosQry(FundPosQuery& query, AccountNo user)
{
	try{
		findApplication(user)->sendFundPosQry(query);
		query.response = TradeApplication::setResponse(RespCode::SUCC,"");
	}catch(NQ::ReqWhenNotLogonError){
		query.response = TradeApplication::setResponse(RespCode::FAIL, "�˻���δ��¼���޷�ִ�н���: ");
	}
}


// ͨ���û�������Ӧapplication
NQ::TradeApplication* NQ::TradeCenter::findApplication(AccountNo user)
{
	Callers::iterator it = callers.find(user);
	return it->second->app;
}

// ͨ���û�������Ӧinitiator
FIX::SocketInitiator* NQ::TradeCenter::findInitiator(AccountNo user)
{
	Callers::iterator it = callers.find(user);
	return it->second->initiator;
}

// ���ûص�����
void NQ::TradeCenter::callBack(OrderResponse &orderResp, AccountNo user){
	this->orderCaller->OnRespLimitOrder(user, orderResp);
}

void NQ::TradeCenter::callBack(ExecuteResponse &execResp, AccountNo user){
	this->orderCaller->OnRespExecuteLimitOrder(user, execResp);
}

void NQ::TradeCenter::callBack(CancelResponse &cancResp, AccountNo user){
	this->orderCaller->OnRespCancelLimitOrder(user, cancResp);
}

void NQ::TradeCenter::callBack(OrderQueryResponse &ordQryResp, AccountNo user){
	this->orderCaller->OnRespOrderQuery(user, ordQryResp);
}

void NQ::TradeCenter::callBack(FundPosQueryMergeResponse &fundPosQryResp, AccountNo user){
	std::cout << "�ص��������" << std::endl;
	std::cout << "user: " << user << std::endl;
	std::cout << "fundPosQryResp.id: " << fundPosQryResp.id << std::endl;
	this->orderCaller->OnRespFundPosQuery(user, fundPosQryResp);
}
