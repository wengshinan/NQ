#include "Demo.h"


Demo::Demo(void)
{
}


Demo::~Demo(void)
{
}

int Demo::ReqLimitOrder(AccountNo user, NQ::LimitedOrder& order)
{
	g_tradeCenter.reqLimitedOrder(order, user);
	return order.response.respCode;
}
int Demo::ReqOrderQuery(AccountNo user, NQ::OrderQuery& query)
{
	g_tradeCenter.reqOrderQuery(query, user);
	return query.response.respCode;
}
int Demo::ReqCancelLimitOrder(AccountNo user, NQ::CancelOrder& order)
{
	g_tradeCenter.reqCancelOrder(order, user);
	return order.response.respCode;
}
int Demo::ReqFundPosQuery(AccountNo user, NQ::FundPosQuery& query)
{
	g_tradeCenter.reqFundPosQry(query, user);
	return query.response.respCode;
}
int Demo::OnRespLimitOrder(AccountNo user, NQ::OrderResponse response)
{
	std::cout<<"OnRespLimitOrder"<<std::endl;
	std::cout<<"response.id: "<<response.id<<std::endl;
	std::cout<<"response.rejectReason: "<<response.rejectReason<<std::endl;
	std::cout<<"response.status: "<<response.status<<std::endl;
	return 1;
}
int Demo::OnRespExecuteLimitOrder(AccountNo user, NQ::ExecuteResponse response)
{
	std::cout<<"OnRespExecuteLimitOrder"<<std::endl;
	std::cout<<"response.id: "<<response.id<<std::endl;
	std::cout<<"response.lastExecNum: "<<response.lastExecNum<<std::endl;
	std::cout<<"response.lastExecPrice: "<<response.lastExecPrice<<std::endl;
	std::cout<<"response.lastExecType: "<<response.lastExecType<<std::endl;
	std::cout<<"response.leavesNum: "<<response.leavesNum<<std::endl;
	std::cout<<"response.num: "<<response.num<<std::endl;
	std::cout<<"response.price: "<<response.price<<std::endl;
	std::cout<<"response.side: "<<response.side<<std::endl;
	std::cout<<"response.status: "<<response.status<<std::endl;
	std::cout<<"response.stock: "<<response.stock<<std::endl;
	return 1;
}
int Demo::OnRespCancelLimitOrder(AccountNo user, NQ::CancelResponse response)
{
	std::cout<<"OnRespCancelLimitOrder"<<std::endl;
	std::cout<<"response.orderId"<<response.orderId<<std::endl;
	std::cout<<"response.oriOrderId"<<response.oriOrderId<<std::endl;
	std::cout<<"response.status"<<response.status<<std::endl;
	std::cout<<"response.rejectReason"<<response.rejectReason<<std::endl;
	return 1;
}
int Demo::OnRespOrderQuery(AccountNo user, NQ::OrderQueryResponse response)
{
	std::cout<<"OnRespOrderQuery"<<std::endl;
	std::cout<<"response.orderId: "<<response.orderId<<std::endl;
	std::cout<<"response.orderNum: "<<response.orderNum<<std::endl;
	std::cout<<"response.orderPrice: "<<response.orderPrice<<std::endl;
	std::cout<<"response.side: "<<response.side<<std::endl;
	std::cout<<"response.status: "<<response.status<<std::endl;
	std::cout<<"response.stock: "<<response.stock<<std::endl;
	std::cout<<"response.totalSum: "<<response.totalSum<<std::endl;
	std::cout<<"response.averageExecPrice: "<<response.averageExecPrice<<std::endl;
	std::cout<<"response.leavesNum: "<<response.leavesNum<<std::endl;
	return 1;
}
int Demo::OnRespFundPosQuery(AccountNo user, NQ::FundPosQueryResponse response)
{
	std::cout<<"OnRespFundPosQuery"<<std::endl;
	std::cout<<"response.id: "<<response.id<<std::endl;
	std::cout<<"response.account: "<<response.account<<std::endl;
	std::cout<<"response.currency: "<<response.currency<<std::endl;
	std::cout<<"response.description: "<<response.description<<std::endl;
	std::cout<<"response.market: "<<response.market<<std::endl;
	std::cout<<"response.stock: "<<response.stock<<std::endl;
	std::cout<<"response.type: "<<response.type<<std::endl;
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
		/*
		int result = demo.ReqLimitOrder(demo.g_user,order);
		std::cout<<result<<std::endl;
		*/
		// 查询资金股份
		while (true)
		{
			std::cout << "1.查询股份" << std::endl;
			std::cout << "2.查询资金" << std::endl;
			std::cout << "3.限价下单" << std::endl;
			std::cout << "4.撤单请求" << std::endl;
			std::cout << "5.持仓查询" << std::endl;
			std::cout << "q.退出查询" << std::endl;
			char ch;
			std::cin >> ch;
			if (ch == '1')
			{
				NQ::FundPosQuery query(0);
				int result = demo.ReqFundPosQuery(demo.g_user,query);
				std::cout<<result<< "  " << query.id << std::endl; 
			} else if (ch == '2')
			{
				NQ::FundPosQuery query(9);			
				int result = demo.ReqFundPosQuery(demo.g_user,query);
				std::cout<<result<< "  " << query.id << std::endl; 
			} else if (ch == '3')
			{
				NQ::LimitedOrder order(
					NQ::TradeSide::Buy,
					"600029",
					1000,
					0.10,
					NQ::Currency::CNY,
					NQ::MarketCode::XSHG
					);
				int result = demo.ReqLimitOrder(demo.g_user,order);
				std::cout<<result<< "  " << order.id <<std::endl;
			} else if (ch == '4')
			{
				std::cout << "请输入需要撤单的单号：" <<std::endl;
				std::string oriId;
				std::cin >> oriId;
				NQ::CancelOrder cancel(oriId, 1000, NQ::TradeSide::Buy,"600029");
				int result = demo.ReqCancelLimitOrder(demo.g_user,cancel);
				std::cout<<result<< "  " << cancel.orderId <<std::endl;
			}
			else if (ch == '5')
			{
				NQ::OrderQuery query("002CF1281432297164412",NQ::TradeSide::Buy, "600029");
				int result = demo.ReqOrderQuery(demo.g_user, query);
				std::cout<<result<< "  " << query.orderId << std::endl; 
			}
			else if (ch == 'q')
			{
				std::cout<<"按任意键退出程序";
				getchar();
				break;
			}
		}
	}
	catch ( std::exception  &e )
	{
		std::cout << e.what();
		return 1;
	}
}