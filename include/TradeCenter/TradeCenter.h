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
		//清空所有用户
		void releaseCallers();

	public:
		//根据用户参数和引擎参数进行初始化
		AccountNo init(const char* pszAppConfigFile, const char* pszEngineConfigFile, IOrderCaller* caller);
		//启动引擎
		void start(AccountNo accountNo);
		//停止引擎
		void stop(AccountNo accountNo);

	public:
		//请求限价报单
		void reqLimitedOrder(LimitedOrder& order, AccountNo user);
		void callBack(OrderResponse &orderResp, AccountNo user);
		void callBack(ExecuteResponse &execResp, AccountNo user);
		//请求撤单
		void reqCancelOrder(CancelOrder& cancel, AccountNo user);
		void callBack(CancelResponse &cancResp, AccountNo user);
		//请求持仓查询
		void reqOrderQuery(OrderQuery& query, AccountNo user);
		void callBack(OrderQueryResponse &ordQryResp, AccountNo user);
		//请求资金股份查询
		void reqFundPosQry(FundPosQuery& query, AccountNo user);
		void callBack(FundPosQueryMergeResponse &fundPosQryResp, AccountNo user);


	private:
		//一个用户对应一个session一个引擎
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
		//用户列表
		Callers callers; 

	private:
		TradeApplication* findApplication(std::string user);
		FIX::SocketInitiator* findInitiator(std::string user);

	};
}


#endif // !TRADE_CENTER_H



