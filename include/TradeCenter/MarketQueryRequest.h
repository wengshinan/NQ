#ifndef MARKET_QUERY_REQUEST_H
#define MARKET_QUERY_REQUEST_H

#include <string>
#include "TDFAPIStruct.h"
#include "TDFAPI.h"
#include <assert.h>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include "TradeError.h"
#include "Util.h"
#include "MarketInterface.h"

namespace NQ{

#define GETRECORD(pBase, TYPE, nIndex) ((TYPE*)((char*)(pBase) + sizeof(TYPE)*(nIndex)))

	enum SubscribType{
		SUBSCRIPTION_SET=0, //����Ϊ�����б��й�Ʊ��
		SUBSCRIPTION_ADD=1,  //���Ӷ����б��й�Ʊ
		SUBSCRIPTION_DEL=2,   //ɾ���б��еĶ���
		SUBSCRIPTION_FULL = 3, //ȫ�г�����
	};

	/*
	enum OUTPUTTYPE
	{
		OUTPUTTYPE_NO,
		OUTPUTTYPE_SCREN=1, 
		OUTPUTTYPE_FILE = 2,
		OUTPUTTYPE_ALL = OUTPUTTYPE_SCREN | OUTPUTTYPE_FILE,
	};
	*/

	struct ConfigSettings
	{
		enum
		{
			//DEFAULT_MAX_MEM_BUF=8*1024*1024,
			//DEFAULT_MAX_WRITE_TIME_GAP=10,
			//DEFAFAULT_OUT_PUT_DEVICE=OUTPUTTYPE_ALL,
			DEFAULT_RECONNECT_COUNT=99999999,
			DEFAULT_RECONNECT_GAP=5,
			DEFAULT_TDF_OPEN_TIME_OUT=30,
			DEFAULT_HEART_BEAT_GAP=10,
			DEFAULT_MISSED_HEAT_BEAT_COUNT=2,
		};

		ConfigSettings(){}
		~ConfigSettings(){}

		char szIP[32];
		short nPort;
		char szUser[32];
		char szPwd[32];
		int nDate;
		int nTime;
		char* szSubscriptions;          //dynamic memory
		char szMarketList[1024];
		char szDataDir[260];            //absolute path
		//int nOutputDevice;
		//bool bOutputCodeTable;
		//unsigned int nMaxMemBuf;        //default 32M for each kind of data(unit:byte)
		//unsigned int nMaxWriteTimeGap;  //default 10's(unit:s)
		unsigned int nReconnectCount;   //
		unsigned int nReconnGap;        //unit:s
		int nDataType;                  //0 means all type.
		unsigned int nCommonLogGap;     //>=1
		unsigned int nMaxConFailCount;	//�����������

		//�����������
		bool bEnableProxy;
		TDF_PROXY_TYPE nProxyType;
		char szProxyHostIp[32];
		short nProxyHostPort;
		char szProxyUser[32];
		char szProxyPassword[32];

		unsigned int nOpenTimeOut;//TDF_Open��ʱʱ��:
		unsigned int nHeartBeatGap; //����ʱ��
		unsigned int nMissedHeartBeatCount;//��ʧ��������

		bool LoadSettings(const std::string& strConfigFile);
		//std::string ToString();
		//static std::string OutputDeviceToStr(OUTPUTTYPE);
		//static std::string DataTypeToStr(DATA_TYPE_FLAG);

		//static TDF_PROXY_TYPE ProxyTypeStr2TDF(int* nIsValid, const std::string& strProxy);
	private:
		//ConfigParser* m_pConfigParser;
		//void RestoreStatus();


	};
	class  MarketQueryRequest
	{
	public:
		//�������ļ�����ȷ�������쳣
		MarketQueryRequest(std::string strConfigFile);
		//�����������ͷ�map�ڴ�
		~MarketQueryRequest();

		//���û����������������ӣ�ֱ�����ӳɹ��򳬳��涨����
		void setEnvironment();
		void getConnection();

		//�������飬��֧��Ʊ��typeΪ��������
		bool subscribMarketData(std::string code, SubscribType type);
		//�������飬��֧��Ʊ������Ϊvector��typeΪ��������
		bool subscribMarketData(std::vector<std::string>& codes, SubscribType type);

		//���õ��÷������ڻص�����ִ��
		void setMarketCaller(NQ_ET::IDataInStream<NQ_ET::SQuote>* marketCaller){
			g_caller = marketCaller;
		}
		// �Ͽ�����
		void close(){
			if (g_hTDF)	TDF_Close(g_hTDF);
		}
		int m_threadCnt;
		int max_threadCnt;
		// д����������־
		void writeLog(std::string content);

	public:
		static CRITICAL_SECTION m_cs;
		static MarketQueryRequest* g_marketReq;
		// ֤ȯ���룬key��ô��룬value��˾����
		static std::map<std::string, std::string> m_marketCode;
		// �������ݣ�key��ô��룬value��������
		static std::map<std::string, TDF_MARKET_DATA> m_marketData;
		// �������ݻص�����
		static void RecvData(THANDLE hTdf, TDF_MSG* pMsgHead);
		// ϵͳ��Ϣ�ص�����
		static void RecvSys(THANDLE hTdf, TDF_MSG* pSysMsg);
		

	private:
		ConfigSettings cfgSettings;

		std::ofstream g_fsLog;
		THANDLE g_hTDF;

		MarketCaller* caller;
		NQ_ET::IDataInStream<NQ_ET::SQuote>* g_caller;

	private:
		void LoadTDFSettings(const ConfigSettings& configObj,  TDF_OPEN_SETTING& settings);
		void LoadProxyTDFSettings(const ConfigSettings& configObj, TDF_PROXY_SETTING& proxy_settings);
		char* DeepCopy(const char*szIn);

		void callBackTick(NQ_ET::SQuote dTick);
		std::string getWindCode(std::string code);

		void saveTodayMarket();
		std::ofstream initDataFile(std::string path, bool isNew);

	private:
		static bool dataModified(TDF_MARKET_DATA recentData);
		static NQ_ET::EListedExchange findMarket(std::string market);
		static NQ_ET::SQuote getTickData(TDF_MARKET_DATA& marketData);
		static NQ_ET::SQuote getTickData(TDF_INDEX_DATA& indexData);


	};

}
#endif // !MARKET_QUERY_REQUEST_H