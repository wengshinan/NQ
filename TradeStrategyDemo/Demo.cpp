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
		// ����һ���޼۱���
		Sleep(1000);
		/*
		int result = demo.ReqLimitOrder(demo.g_user,order);
		std::cout<<result<<std::endl;
		*/
		// ��ѯ�ʽ�ɷ�
		while (true)
		{
			std::cout << "1.��ѯ�ɷ�" << std::endl;
			std::cout << "2.��ѯ�ʽ�" << std::endl;
			std::cout << "3.�޼��µ�" << std::endl;
			std::cout << "4.��������" << std::endl;
			std::cout << "5.�ֲֲ�ѯ" << std::endl;
			std::cout << "0.�˳���ѯ" << std::endl;
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
				std::cout << "��������Ҫ�����ĵ��ţ�" <<std::endl;
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
				std::cout<<"��������˳�fix����";
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
		std::cout << "===========����������붩��============" << std::endl ;
		getchar();
		while (true)
		{
			int flag;
			demo.g_marketCenter->writeLog("-----------------------------");
			std::cout << "1.ȫ�г���������" << std::endl;
			std::cout << "2.���ö�������" << std::endl;
			std::cout << "3.���Ӷ�������" << std::endl;
			std::cout << "4.ɾ����������" << std::endl;
			std::cout << "5.���½�������" << std::endl;
			std::cout << "6.�鿴����״̬" << std::endl;
			std::cout << "0.�˳���������" << std::endl;
			std::cout << "��ѡ��" ;
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
				std::cout << "�����趩�ĵ�֤ȯ���룺" ;
				std::cin >> stock;
				if (demo.g_marketCenter->g_marketReq->subscribMarketData(stock, NQ::SubscribType::SUBSCRIPTION_SET))
					result = 1;
				else result = 0;
			} else if (flag == 3)
			{
				std::string stock;
				std::cout << "���������Ӷ��ĵ�֤ȯ���룺" ;
				std::cin >> stock;
				if (demo.g_marketCenter->g_marketReq->subscribMarketData(stock, NQ::SubscribType::SUBSCRIPTION_ADD))
					result = 1;
				else result = 0;
			} else if (flag == 4)
			{
				std::string stock;
				std::cout << "������ɾ�����ĵ�֤ȯ���룺" ;
				std::cin >> stock;
				if (demo.g_marketCenter->g_marketReq->subscribMarketData(stock, NQ::SubscribType::SUBSCRIPTION_DEL))
					result = 1;
				else result = 0;
			} else if (flag == 5)
			{
				std::cout << "��������......." << std::endl;
				demo.g_marketCenter->getConnection();
				Sleep(3*1000);
				if (demo.g_marketCenter->isConnected()) std::cout << "������" << std::endl;
				else std::cout << "δ����" << std::endl;
			} else 
			{
				if (demo.g_marketCenter->isConnected()) std::cout << "������" << std::endl;
				else std::cout << "δ����" << std::endl;
			}
			if (flag < 5 && flag > 0){
				if (result == 1) std::cout << std::endl << "����ɹ�" << std::endl;
				else std::cout << std::endl << "����ʧ��" << std::endl;
			}
			std::cout << "===============================================" << std::endl;

		}

		if(getchar()){
			std::cout << "�Ͽ�����" << std::endl;
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
		std::cout << "1 ����fix" << std::endl;
		std::cout << "2 ����wind" << std::endl;
		std::cout << "0 �˳�����" << std::endl;
		std::cout << "ѡ��";
		int flag = 0;
		std::cin >> flag;
		if (flag == 1)
		{
			//����fix
			testTradeCenter();
		} else if (flag == 2){
			//��������
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
	std::cout << "�����������" << std::endl;
	getchar();
}