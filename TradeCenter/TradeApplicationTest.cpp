#include <stdio.h>

// quickfix引擎配置文件
const char *pszAppConfigFile = "E:/Workspace/TradeCenter/ThirdParty/AppConfig.ini";
const char *pszEngineConfigFile = "E:/Workspace/TradeCenter/ThirdParty/tradeclient.cfg";

		/*
int main()
{
	try
	{
		Demo demo;
		demo.LoadUser();

		NQ::TradeCenter tradeCenter;

		AccountNo user = tradeCenter.init(pszAppConfigFile,pszEngineConfigFile);
		std::cout<<user<<std::endl;

		FIX::SessionSettings settings( pszEngineConfigFile );

		NQ::TradeApplication application(pszAppConfigFile);
		FIX::FileStoreFactory storeFactory( settings );  // 文件方式存储消息
		FIX::FileLogFactory logFactory( settings );      // 文件方式存储日志
		FIX::SocketInitiator initiator( application, storeFactory, settings, logFactory );


		// 启动引擎
		initiator.start();
		// 发送一单限价报单
		NQ::LimitedOrder order(
			NQ::TradeSide::Buy,
			"2030",
			200,
			100.00,
			NQ::Currency::CNY,
			NQ::MarketCode::XSHE
			);
		int i=0;
		while (!application.isConnected()&&i++<10)
		{
			Sleep(1000);
		}
		//if (application.isConnected()) 
		application.sendLimitedOrder(order);

		std::cout<<"按任意键退出程序";
		getchar();
		// 停止引擎
		initiator.stop();
		return 0;
	}
	catch ( std::exception  &e )
	{
		std::cout << e.what();
		return 1;
	}
}
		*/