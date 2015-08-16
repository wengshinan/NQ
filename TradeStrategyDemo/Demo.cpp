#include "Demo.h"


Demo::Demo(void)
{
}


Demo::~Demo(void)
{
	if (g_marketCenter) g_marketCenter->close();
	delete g_marketCenter;
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
int Demo::OnRespFundPosQuery(AccountNo user, NQ::FundPosQueryMergeResponse response)
{
	std::cout<<"OnRespFundPosQuery"<<std::endl;
	std::cout<<"response.id: "<<response.id<<std::endl;
	std::cout<<"response.type: "<<response.type<<std::endl;
	std::cout<<"response.description: "<<response.description<<std::endl;
	std::cout<<"response.stockFundPos quantity: "<<response.stockFundPoses.size()<<std::endl;
	return 1;
}

int Demo::LoadUser()
{
	g_user = g_tradeCenter.init(
		"AppConfig.ini",
		"tradeclient.cfg",
		this);
	std::cout<<g_user<<std::endl;

	g_tradeCenter.start(g_user);
	return 1;
}

int Demo::UnloadUser(AccountNo user)
{
	return 1;
}

int testTradeCenter(){
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
			std::cout << "0.退出查询" << std::endl;
			char ch;
			std::cin >> ch;
			if (ch == '1')
			{
				NQ::FundPosQuery query(0);
				int result = demo.ReqFundPosQuery(demo.g_user,query);
				std::cout<<result<< "  " << query.id << std::endl; 
				if (result == NQ::RespCode::FAIL)
				{
					std::cout << query.response.errorMessage << std::endl;
				}
			} else if (ch == '2')
			{
				NQ::FundPosQuery query(9);			
				int result = demo.ReqFundPosQuery(demo.g_user,query);
				std::cout<<result<< "  " << query.id << std::endl; 
				if (result == NQ::RespCode::FAIL)
				{
					std::cout << query.response.errorMessage << std::endl;
				}
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
				if (result == NQ::RespCode::FAIL)
				{
					std::cout << order.response.errorMessage << std::endl;
				}
			} else if (ch == '4')
			{
				std::cout << "请输入需要撤单的单号：" <<std::endl;
				std::string oriId;
				std::cin >> oriId;
				NQ::CancelOrder cancel(oriId, 1000, NQ::TradeSide::Buy,"600029");
				int result = demo.ReqCancelLimitOrder(demo.g_user,cancel);
				std::cout<<result<< "  " << cancel.orderId <<std::endl;
				if (result == NQ::RespCode::FAIL)
				{
					std::cout << cancel.response.errorMessage << std::endl;
				}
			}
			else if (ch == '5')
			{
				NQ::OrderQuery query("002CF1281432297164412",NQ::TradeSide::Buy, "600029");
				int result = demo.ReqOrderQuery(demo.g_user, query);
				std::cout<<result<< "  " << query.orderId << std::endl; 
				if (result == NQ::RespCode::FAIL)
				{
					std::cout << query.response.errorMessage << std::endl;
				}
			}
			else if (ch == '0')
			{
				std::cout<<"按任意键退出fix测试";
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
	return 0;
}


int Demo::init(std::string marketConfigFile)
{
	NQ::MarketQueryRequest* g_marketRequest = new NQ::MarketQueryRequest(marketConfigFile);
	g_marketCenter =g_marketRequest;
	g_marketCenter->setEnvironment();
	g_marketCenter->getConnection();
	g_marketCenter->setMarketCaller(this);
	return 1;
}

int Demo::reqMarketQuery()
{
	g_marketCenter->subscribMarketData("",NQ::SubscribType::SUBSCRIPTION_FULL);
	return 1;
}

int Demo::onRespMarketQuery(NQ_ET::SQuote tickData)
{
	std::cout << tickData.StkCode << " " << tickData.CurrentTime <<": " << tickData.LastPrice << std::endl;
	return 1;
}

bool Demo::OnData(NQ_ET::SQuote& data)
{
	std::cout << data.StkCode << " " << data.CurrentTime <<": " << data.LastPrice << std::endl;
	return true;
}

int testMarketQuery()
{
	Demo demo;
	int result = 0;
	try{
		result = demo.init("TDFEasyDemo.ini");
		//result = demo.reqMarketQuery();
		std::cout << "===========按任意键进入订阅============" << std::endl ;
		getchar();
		while (true)
		{
			int flag;
			demo.g_marketCenter->writeLog("-----------------------------");
			std::cout << "1.全市场订阅行情" << std::endl;
			std::cout << "2.设置订阅行情" << std::endl;
			std::cout << "3.增加订阅行情" << std::endl;
			std::cout << "4.删除订阅行情" << std::endl;
			std::cout << "5.重新建立连接" << std::endl;
			std::cout << "6.查看连接状态" << std::endl;
			std::cout << "0.退出订阅行情" << std::endl;
			std::cout << "请选择：" ;
			std::cin >> flag;

			result = 0;

			if (flag == 0)
			{
				break;
			} else if (flag == 1)
			{
				if (demo.g_marketCenter->g_marketReq->subscribMarketData("", NQ::SubscribType::SUBSCRIPTION_FULL))
					result = 1;
				else result = 0;
			} else if (flag == 2)
			{
				std::string stock;
				std::cout << "输入需订阅的证券代码：" ;
				std::cin >> stock;
				if (demo.g_marketCenter->g_marketReq->subscribMarketData(stock, NQ::SubscribType::SUBSCRIPTION_SET))
					result = 1;
				else result = 0;
			} else if (flag == 3)
			{
				std::string stock;
				std::cout << "输入需增加订阅的证券代码：" ;
				std::cin >> stock;
				if (demo.g_marketCenter->g_marketReq->subscribMarketData(stock, NQ::SubscribType::SUBSCRIPTION_ADD))
					result = 1;
				else result = 0;
			} else if (flag == 4)
			{
				std::string stock;
				std::cout << "输入需删除订阅的证券代码：" ;
				std::cin >> stock;
				if (demo.g_marketCenter->g_marketReq->subscribMarketData(stock, NQ::SubscribType::SUBSCRIPTION_DEL))
					result = 1;
				else result = 0;
			} else if (flag == 5)
			{
				std::cout << "请求连接......." << std::endl;
				demo.g_marketCenter->getConnection();
				Sleep(3*1000);
				if (demo.g_marketCenter->isConnected()) std::cout << "已连接" << std::endl;
				else std::cout << "未连接" << std::endl;
			} else 
			{
				if (demo.g_marketCenter->isConnected()) std::cout << "已连接" << std::endl;
				else std::cout << "未连接" << std::endl;
			}
			if (flag < 5 && flag > 0){
				if (result == 1) std::cout << std::endl << "处理成功" << std::endl;
				else std::cout << std::endl << "处理失败" << std::endl;
			}
			std::cout << "===============================================" << std::endl;

		}

		if(getchar()){
			std::cout << "断开连接" << std::endl;
			std::cout << demo.g_marketCenter->m_marketData.size() << std::endl;
			std::cout << demo.g_marketCenter->m_threadCnt << std::endl;
		}
	}catch(std::exception e){
		std::cout << e.what() << std::endl;
		return result;
	}

	return result;
}

int main()
{
	while(true){
		std::cout << "------------" << std::endl;
		std::cout << "1 测试fix" << std::endl;
		std::cout << "2 测试wind" << std::endl;
		std::cout << "0 退出测试" << std::endl;
		std::cout << "选择：";
		int flag = 0;
		std::cin >> flag;
		if (flag == 1)
		{
			//测试fix
			testTradeCenter();
		} else if (flag == 2){
			//测试行情
			testMarketQuery();
		} else {
			break;
		}
		std::cout << std::endl << std::endl;
	}

	/*
	std::string szCode = "000001.SZ";
	int i1 = szCode.find('.');
	int i2 = szCode.length();
	std::string market = szCode.substr(szCode.find('.')+1,szCode.length()-i1-1);
	std::cout << market << std::endl;
	*/
	std::cout << "按任意键结束" << std::endl;
	getchar();
}