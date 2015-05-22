#pragma once
#include "TradeCenter/TradeCenter.h"


class Demo : public NQ::IUser, public NQ::IOrderCaller
{
public:
	Demo(void);
	~Demo(void);

	int ReqLimitOrder(AccountNo user, NQ::LimitedOrder& order);
	int ReqCancelLimitOrder(AccountNo user, NQ::CancelOrder& order);
	int ReqOrderQuery(AccountNo user, NQ::OrderQuery& query);
	int ReqFundPosQuery(AccountNo user, NQ::FundPosQuery& query);

	int OnRespLimitOrder(AccountNo user, NQ::OrderResponse response);
	int OnRespExecuteLimitOrder(AccountNo user, NQ::ExecuteResponse response);
	int OnRespCancelLimitOrder(AccountNo user, NQ::CancelResponse response);
	int OnRespOrderQuery(AccountNo user, NQ::OrderQueryResponse response);
	int OnRespFundPosQuery(AccountNo user, NQ::FundPosQueryResponse response);



	int LoadUser();
	int UnloadUser(AccountNo user);

	AccountNo g_user;
	NQ::TradeCenter g_tradeCenter;
};

