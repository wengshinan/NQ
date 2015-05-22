#ifndef TRADE_INTERFACE_H
#define TRADE_INTERFACE_H

#include "TradeEntity.h"

namespace NQ{
	__interface IOrderCaller
	{
	public:
		//限价下单请求，应调用TradeCenter::reqLimitedOrder()，以下类似
		virtual int ReqLimitOrder(AccountNo user, LimitedOrder& order)=0;
		//撤单请求
		virtual int ReqCancelLimitOrder(AccountNo user, CancelOrder& order)=0;
		//持仓查询
		virtual int ReqOrderQuery(AccountNo user, OrderQuery& query)=0;
		//资金股份查询
		virtual int ReqFundPosQuery(AccountNo user, FundPosQuery& query)=0;
		

		//限价下单回调
		virtual int OnRespLimitOrder(AccountNo user, OrderResponse response)=0;
		//限价下单成交回调
		virtual int OnRespExecuteLimitOrder(AccountNo user, ExecuteResponse response)=0;
		//撤单回报回调
		virtual int OnRespCancelLimitOrder(AccountNo user, CancelResponse response)=0;
		//持仓查询回报回调
		virtual int OnRespOrderQuery(AccountNo user, OrderQueryResponse response)=0;
		//资金股份查询回报回调
		virtual int OnRespFundPosQuery(AccountNo user, FundPosQueryResponse response)=0;
	};

	__interface IUser
	{
	public:
		virtual int LoadUser()=0; //通过配置文件来初始化引擎，TradeCenter::init(); start()
		virtual int UnloadUser(AccountNo user)=0;  //TradeCenter::stop()
	};

}


#endif // !TRADE_INTERFACE_H
