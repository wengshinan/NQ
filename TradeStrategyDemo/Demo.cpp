#include "Demo.h"


Demo::Demo(void)
{
}


Demo::~Demo(void)
{
}

int Demo::ReqLimitOrder(AccountNo user, NQ::LimitedOrder order)
{
	g_tradeCenter.reqLimitedOrder(order, user);
	return order.response.respCode;
}
int Demo::ReqOrderQuery(AccountNo user, NQ::OrderQuery query)
{
	return 1;
}
int Demo::ReqCancelLimitOrder(AccountNo user, NQ::CancelOrder order)
{
	return 1;
}
int Demo::ReqFundPosQuery(AccountNo user, NQ::FundPosQuery query)
{
	return 1;
}
int Demo::OnRespLimitOrder(AccountNo user, NQ::OrderResponse response)
{
	std::cout<<"OnRespLimitOrder"<<std::endl;
	return 1;
}
int Demo::OnRespExecuteLimitOrder(AccountNo user, NQ::ExecuteResponse response)
{
	std::cout<<"OnRespExecuteLimitOrder"<<std::endl;
	return 1;
}
int Demo::OnRespCancelLimitOrder(AccountNo user, NQ::CancelResponse response)
{
	return 1;
}
int Demo::OnRespOrderQuery(AccountNo user, NQ::OrderQueryResponse response)
{
	return 1;
}
int Demo::OnRespFundPosQuery(AccountNo user, NQ::FundPosQueryResponse response)
{
	return 1;
}

int Demo::LoadUser()
{
	g_user = g_tradeCenter.init(
		"E:/Workspace/TradeCenter/ThirdParty/AppConfig.ini",
		"E:/Workspace/TradeCenter/ThirdParty/tradeclient.cfg",
		this);
	std::cout<<g_user<<std::endl;
	
	g_tradeCenter.start(g_user);
	return 1;
}

int Demo::UnloadUser(AccountNo user)
{
	return 1;
}


int main()
{
	try
	{
		Demo demo;
		demo.LoadUser();
		// 发送一单限价报单
		Sleep(1000);
		NQ::LimitedOrder order(
			NQ::TradeSide::Buy,
			"2030",
			200,
			100.00,
			NQ::Currency::CNY,
			NQ::MarketCode::XSHE
			);

		int result = demo.ReqLimitOrder(demo.g_user,order);
		std::cout<<result<<std::endl;
		std::cout<<"按任意键退出程序";
		getchar();
	}
	catch ( std::exception  &e )
	{
		std::cout << e.what();
		return 1;
	}
}