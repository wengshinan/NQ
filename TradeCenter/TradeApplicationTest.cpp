#include <stdio.h>

// quickfix���������ļ�
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
		FIX::FileStoreFactory storeFactory( settings );  // �ļ���ʽ�洢��Ϣ
		FIX::FileLogFactory logFactory( settings );      // �ļ���ʽ�洢��־
		FIX::SocketInitiator initiator( application, storeFactory, settings, logFactory );


		// ��������
		initiator.start();
		// ����һ���޼۱���
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

		std::cout<<"��������˳�����";
		getchar();
		// ֹͣ����
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