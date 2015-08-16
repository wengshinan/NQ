#ifndef TRADE_CENTER_H
#define TRADE_CENTER_H

#include "TradeApplication.h"
#include "TradeInterface.h"

namespace NQ{
	class TradeCenter
	{
	public:
		TradeCenter(void){
				//callers.clear();	
		};
		~TradeCenter(void){};

	public:
		//��������û�
		void releaseCallers();

	public:
		//�����û�����������������г�ʼ��
		AccountNo init(const char* pszAppConfigFile, const char* pszEngineConfigFile, IOrderCaller* caller);
		//��������
		void start(AccountNo accountNo);
		//ֹͣ����
		void stop(AccountNo accountNo);

	public:
		//�����޼۱���
		void reqLimitedOrder(LimitedOrder& order, AccountNo user);
		void callBack(OrderResponse &orderResp, AccountNo user);
		void callBack(ExecuteResponse &execResp, AccountNo user);
		//���󳷵�
		void reqCancelOrder(CancelOrder& cancel, AccountNo user);
		void callBack(CancelResponse &cancResp, AccountNo user);
		//����ֲֲ�ѯ
		void reqOrderQuery(OrderQuery& query, AccountNo user);
		void callBack(OrderQueryResponse &ordQryResp, AccountNo user);
		//�����ʽ�ɷݲ�ѯ
		void reqFundPosQry(FundPosQuery& query, AccountNo user);
		void callBack(FundPosQueryMergeResponse &fundPosQryResp, AccountNo user);


	private:
		//һ���û���Ӧһ��sessionһ������
		struct AppConfiguration{
			NQ::TradeApplication* app;
			FIX::FileStoreFactory* store;
			FIX::FileLogFactory* log;
			FIX::SessionSettings* setting;
			FIX::SocketInitiator* initiator;
			AppConfiguration(
				NQ::TradeApplication* app,
				FIX::FileStoreFactory* store,
				FIX::FileLogFactory* log,
				FIX::SessionSettings* setting,
				FIX::SocketInitiator* initiator)
				:app(app),
				store(store),
				log(log),
				setting(setting),
				initiator(initiator){}
			AppConfiguration(){}
		};
		typedef std::map<AccountNo, AppConfiguration*> Callers;
		IOrderCaller *orderCaller;
		//�û��б�
		Callers callers; 

	private:
		TradeApplication* findApplication(std::string user);
		FIX::SocketInitiator* findInitiator(std::string user);

	};
}


#endif // !TRADE_CENTER_H



