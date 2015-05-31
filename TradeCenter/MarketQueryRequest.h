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
		SUBSCRIPTION_SET=0, //设置为订阅列表中股票，
		SUBSCRIPTION_ADD=1,  //增加订阅列表中股票
		SUBSCRIPTION_DEL=2,   //删除列表中的订阅
		SUBSCRIPTION_FULL = 3, //全市场订阅
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
		unsigned int nMaxConFailCount;	//最大重连次数

		//代理连接类别
		bool bEnableProxy;
		TDF_PROXY_TYPE nProxyType;
		char szProxyHostIp[32];
		short nProxyHostPort;
		char szProxyUser[32];
		char szProxyPassword[32];

		unsigned int nOpenTimeOut;//TDF_Open超时时间:
		unsigned int nHeartBeatGap; //心跳时间
		unsigned int nMissedHeartBeatCount;//丢失心跳次数

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
		//若配置文件不正确，将报异常
		MarketQueryRequest(std::string strConfigFile);
		//析构函数，释放map内存
		~MarketQueryRequest();

		//配置环境变量并建立链接，直到连接成功或超出规定次数
		void setEnvironment();
		void getConnection();

		//订阅行情，单支股票，type为订阅类型
		bool subscribMarketData(std::string code, SubscribType type);
		//订阅行情，多支股票，保存为vector，type为订阅类型
		bool subscribMarketData(std::vector<std::string>& codes, SubscribType type);

		//设置调用方，便于回调函数执行
		void setMarketCaller(NQ_ET::IDataInStream<NQ_ET::SQuote>* marketCaller){
			g_caller = marketCaller;
		}

	public:
		// 证券代码，key万得代码，value代码元数据
		static std::map<std::string, TDF_CODE> m_marketCode;
		// 行情数据，key万得代码，value行情数据
		static std::map<std::string, TDF_MARKET_DATA> m_marketData;
		// 行情数据回调函数
		static void RecvData(THANDLE hTdf, TDF_MSG* pMsgHead);
		// 系统消息回调函数
		static void RecvSys(THANDLE hTdf, TDF_MSG* pSysMsg);
		

	private:
		static MarketQueryRequest* g_marketReq;
		ConfigSettings cfgSettings;

		std::ofstream g_fsLog;
		THANDLE g_hTDF;

	private:
		void LoadTDFSettings(const ConfigSettings& configObj,  TDF_OPEN_SETTING& settings);
		void LoadProxyTDFSettings(const ConfigSettings& configObj, TDF_PROXY_SETTING& proxy_settings);
		char* DeepCopy(const char*szIn);

		static bool dataModified(TDF_MARKET_DATA recentData);
		void callBackTick(std::string szWindCode);
		std::string getWindCode(std::string code);

		MarketCaller* caller;
		NQ_ET::IDataInStream<NQ_ET::SQuote>* g_caller;
	};

}
#endif // !MARKET_QUERY_REQUEST_H