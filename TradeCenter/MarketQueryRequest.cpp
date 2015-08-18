#include "MarketQueryRequest.h"

std::map<std::string, std::string> NQ::MarketQueryRequest::m_marketCode;
std::map<std::string, TDF_MARKET_DATA> NQ::MarketQueryRequest::m_marketData;
std::map<std::string, TDF_INDEX_DATA> NQ::MarketQueryRequest::m_indexData;
NQ::MarketQueryRequest* NQ::MarketQueryRequest::g_marketReq;
CRITICAL_SECTION NQ::MarketQueryRequest::m_cs;

bool NQ::ConfigSettings::LoadSettings(const std::string& strConfigFile){
	std::fstream cfgFile;
	cfgFile.open(strConfigFile);
	if (!cfgFile.is_open())
	{
		std::cout << "can not open cfg file" << std::endl;
		return false;
	}
	char tmp[1000];
	std::map <std::string, std::string> key2value;

	//读入配置
	while(!cfgFile.eof())
	{
		cfgFile.getline(tmp, 1000);
		std::string line(tmp);
		size_t pos = line.find('=');
		size_t end = line.find('#');
		if(pos==std::string::npos) continue;

		std::string key = Util::trim(line.substr(0,pos));
		std::string value;
		if(end==std::string::npos) value = Util::trim(line.substr(pos+1));
		else value = pos+1>=end?"":Util::trim(line.substr(pos+1,end-pos-1));

		key2value.insert(std::map<std::string, std::string>::value_type(key, value));
	}
	cfgFile.close();

	std::map<std::string, std::string>::iterator it;

	//设置各个配置
	{
		it=key2value.find("IP");
		if (it!=key2value.end()) strncpy_s(szIP, it->second.c_str(), 31);
		else return false;

		it=key2value.find("Port");
		if (it!=key2value.end()) nPort = atoi(it->second.c_str());
		else return false;

		it=key2value.find("User");
		if (it!=key2value.end()) strncpy_s(szUser, it->second.c_str(), 31);
		else return false;

		it=key2value.find("Password");
		if (it!=key2value.end()) strncpy_s(szPwd, it->second.c_str(), 31);
		else return false;

		it=key2value.find("Date");
		if (it!=key2value.end()) nDate = atoi(it->second.c_str());
		else return false;

		it=key2value.find("Time");
		if (it!=key2value.end()) nTime = atoi(it->second.c_str());
		else return false;

		it=key2value.find("StockList");
		if (it!=key2value.end()) {
			szSubscriptions = new char[it->second.length()+1];
			strncpy_s(szSubscriptions,it->second.length()+1,it->second.c_str(),it->second.length());
		}
		else return false;

		it=key2value.find("MarketList");
		if (it!=key2value.end()) strncpy_s(szMarketList, it->second.c_str(), 31);
		else return false;

		it=key2value.find("DataDir");
		if (it!=key2value.end()) strncpy_s(szDataDir, it->second.c_str(), 260);
		else return false;

		/*
		it=key2value.find("OutputDest");
		if (it!=key2value.end()){
		nOutputDevice = OUTPUTTYPE_NO;
		std::string value;
		value = it->second;
		int offset = value.find(';');
		if (offset!=std::string::npos &&
		((value.substr(0,offset)=="SCREEN"&&value.substr(offset+1)=="CSV") || 
		(value.substr(0,offset)=="CSV"&&value.substr(offset+1)=="SCREEN"))){
		nOutputDevice |= OUTPUTTYPE_SCREN;
		nOutputDevice |= OUTPUTTYPE_FILE;
		}else if (value== "SCREEN")
		{
		nOutputDevice |= OUTPUTTYPE_SCREN;
		}
		else if (value=="CSV")
		{
		nOutputDevice |= OUTPUTTYPE_FILE;
		}
		}
		else return false;

		it=key2value.find("OutputCodeTable");
		if (it!=key2value.end()) bOutputCodeTable = atoi(it->second.c_str());
		else return false;

		it=key2value.find("CommonLogGap");
		if (it!=key2value.end()) nCommonLogGap = atoi(it->second.c_str());
		else return false;

		it=key2value.find("MaxMemBuf");
		if (it!=key2value.end()) nMaxMemBuf = atoi(it->second.c_str());
		else return false;

		it=key2value.find("MaxWriteTimeGap");
		if (it!=key2value.end()) nMaxWriteTimeGap = atoi(it->second.c_str());
		else return false;
		*/

		it=key2value.find("ReconnectCount");
		if (it!=key2value.end()) nReconnectCount = atoi(it->second.c_str());
		else return false;

		it=key2value.find("nReconnectGap");
		if (it!=key2value.end()) nReconnGap = atoi(it->second.c_str());
		else return false;

		it=key2value.find("DataType");
		if (it!=key2value.end()) nDataType = atoi(it->second.c_str());
		else return false;

		it=key2value.find("OpenTimeOut");
		if (it!=key2value.end()) nOpenTimeOut = atoi(it->second.c_str());
		else return false;

		it=key2value.find("HeartBeatGap");
		if (it!=key2value.end()) nHeartBeatGap = atoi(it->second.c_str());
		else return false;

		it=key2value.find("MissHeartBeatCount");
		if (it!=key2value.end()) nMissedHeartBeatCount = atoi(it->second.c_str());
		else return false;

		it=key2value.find("EnableProxy");
		if (it!=key2value.end()) bEnableProxy = atoi(it->second.c_str());
		else return false;

		it=key2value.find("ProxyType");
		if (it!=key2value.end()){
			std::string value = it->second;
			if (value == "SOCK4") nProxyType = TDF_PROXY_SOCK4;
			else if (value == "SOCK4A") nProxyType = TDF_PROXY_SOCK4A;
			else if (value == "SOCK5") nProxyType = TDF_PROXY_SOCK5;
			else if (value == "HTTP11") nProxyType = TDF_PROXY_HTTP11;
		}
		else return false;

		it=key2value.find("ProxyHostIP");
		if (it!=key2value.end()) strncpy_s(szProxyHostIp, it->second.c_str(), 31);
		else return false;

		it=key2value.find("ProxyHostPort");
		if (it!=key2value.end()) nProxyHostPort = atoi(it->second.c_str());
		else return false;

		it=key2value.find("ProxyUser");
		if (it!=key2value.end()) strncpy_s(szProxyUser, it->second.c_str(), 31);
		else return false;

		it=key2value.find("ProxyPassword");
		if (it!=key2value.end()) strncpy_s(szProxyPassword, it->second.c_str(), 31);
		else return false;

		it=key2value.find("MaxFailCount");
		if (it!=key2value.end()) nMaxConFailCount = atoi(it->second.c_str());
		else return false;

		it=key2value.find("MaxThreadCount");
		if (it!=key2value.end()) NQ::MarketQueryRequest::g_marketReq->max_threadCnt = atoi(it->second.c_str());
		else return false;
	}

	key2value.clear();
	return true;
}

std::ofstream NQ::MarketQueryRequest::initDataFile(std::string path, bool isNew)
{
	if (isNew)
	{
		remove(path.c_str());
	}
	std::ofstream outData;
	outData.open(path.c_str(), std::ios_base::out);
	return outData;
}

NQ::MarketQueryRequest::MarketQueryRequest(std::string strConfigFile){
	g_marketReq = this;
	bool bLoadSettings = cfgSettings.LoadSettings(strConfigFile);
	if(!bLoadSettings) {
		throw ReadConfigFailError(strConfigFile);
		return;
	}

	char today[32]  ="";
	time_t time = std::time(NULL);
	tm temptm;
	localtime_s(&temptm, &time);
	sprintf_s(today,"%d年%02d月%02d日",temptm.tm_year+1900,temptm.tm_mon+1,temptm.tm_mday);  
	std::string strDataPath(cfgSettings.szDataDir, 0, sizeof(cfgSettings.szDataDir)-1);
	strDataPath += std::string(today);
	strDataPath += "日志文件.txt";

	g_fsLog = initDataFile(strDataPath, false);
	//g_fsData.open(strDataPath.c_str(), std::ios_base::out);
	if (!g_fsLog.is_open())
	{
		throw OpenFileFailError(strDataPath.c_str());
		return;
	}

	g_hTDF = NULL;
	m_threadCnt = 0;

	InitializeCriticalSection(&m_cs);
}

NQ::MarketQueryRequest::~MarketQueryRequest(){
	this->saveTodayMarket();
	g_fsLog.flush();
	g_fsLog.close();
	m_marketCode.clear();
	m_marketData.clear();
	m_indexData.clear();
	DeleteCriticalSection(&m_cs);
}

void NQ::MarketQueryRequest::setEnvironment()
{

	TDF_SetEnv(TDF_ENVIRON_HEART_BEAT_INTERVAL, cfgSettings.nHeartBeatGap);
	TDF_SetEnv(TDF_ENVIRON_MISSED_BEART_COUNT, cfgSettings.nMissedHeartBeatCount);
	TDF_SetEnv(TDF_ENVIRON_OPEN_TIME_OUT, cfgSettings.nOpenTimeOut);
	g_fsLog << "环境配置成功" << std::endl;
}

void NQ::MarketQueryRequest::getConnection()
{
	//同步配置，包括回调函数等
	TDF_OPEN_SETTING open_settings = {0};
	LoadTDFSettings(cfgSettings, open_settings);
	/*
	g_nMaxBuf = cfgSettings.nMaxMemBuf;
	g_nMaxWriteTimeGap = cfgSettings.nMaxWriteTimeGap;
	g_nOutputDevice = cfgSettings.nOutputDevice;
	g_nCommonLogGap = cfgSettings.nCommonLogGap;
	g_nOutputCodeTable = cfgSettings.bOutputCodeTable;
	*/

	TDF_ERR nErr = TDF_ERR_SUCCESS;

	//同步连接开启，正常和代理两种方式
	TDF_PROXY_SETTING proxy_settings = {(TDF_PROXY_TYPE)0};
	if (cfgSettings.bEnableProxy)
	{
		LoadProxyTDFSettings(cfgSettings, proxy_settings);
		g_hTDF = TDF_OpenProxy(&open_settings, &proxy_settings, &nErr);
	}
	else
	{
		g_hTDF = TDF_Open(&open_settings, &nErr);
		//std::cout << "test" << std::endl;
	}

	//delete[] open_settings.szMarkets;
	//delete[] open_settings.szSubScriptions;


	//重连至成功或超过最大重连次数
	int connCnt = 0;
	while (nErr == TDF_ERR_NETWORK_ERROR)
	{
		//Print("TDF_Open 连接 %s:%s user:%s, password:%s 失败，3秒之后重连\n", open_settings.szIp, open_settings.szPort, open_settings.szUser, open_settings.szPwd);
		Sleep(3*1000);

		//LoadTDFSettings(cfgSettings, open_settings);

		/*
		g_nMaxBuf = cfgSettings.nMaxMemBuf;
		g_nMaxWriteTimeGap = cfgSettings.nMaxWriteTimeGap;
		g_nOutputDevice = cfgSettings.nOutputDevice;
		g_nCommonLogGap = cfgSettings.nCommonLogGap;
		*/

		if (cfgSettings.bEnableProxy)
		{
			//TDF_PROXY_SETTING proxy_settings = {(TDF_PROXY_TYPE)0};
			//LoadProxyTDFSettings(cfgSettings, proxy_settings);
			g_hTDF = TDF_OpenProxy(&open_settings, &proxy_settings, &nErr);
		}
		else
		{
			g_hTDF = TDF_Open(&open_settings, &nErr);
		}

		//Print("TDF_Open returned:%s\n", GetErrStr(nErr));
		//delete[] open_settings.szMarkets;
		//delete[] open_settings.szSubScriptions;

		if (++connCnt>cfgSettings.nMaxConFailCount)
		{
			//delete[] open_settings.szMarkets;
			//delete[] open_settings.szSubScriptions;
			writeLog("连接服务器失败");
			throw ConnectFailError();
			return;
		}

		//delete[] open_settings.szMarkets;
		//delete[] open_settings.szSubScriptions;
	}
}


void NQ::MarketQueryRequest::LoadTDFSettings(const ConfigSettings& configObj,  TDF_OPEN_SETTING& settings)
{
	memset(&settings, 0, sizeof(settings));

	strncpy_s(settings.szIp, configObj.szIP, sizeof(settings.szIp)-1);
	_snprintf(settings.szPort, sizeof(settings.szPort)-1, "%d", configObj.nPort);
	strncpy_s(settings.szUser, configObj.szUser, sizeof(settings.szUser)-1);
	strncpy_s(settings.szPwd, configObj.szPwd, sizeof(settings.szPwd)-1);

	settings.nReconnectCount = configObj.nReconnectCount;
	settings.nReconnectGap = configObj.nReconnGap;

	settings.pfnMsgHandler = RecvData;
	settings.pfnSysMsgNotify = RecvSys;

	settings.nProtocol = 0;
	settings.szMarkets = DeepCopy(configObj.szMarketList);
	settings.szSubScriptions = DeepCopy(configObj.szSubscriptions);

	settings.nDate = configObj.nDate;
	settings.nTime = configObj.nTime;

	settings.nTypeFlags = configObj.nDataType;
	//settings.nTypeFlags = DATA_TYPE_MARKET;
}

void NQ::MarketQueryRequest::LoadProxyTDFSettings(const ConfigSettings& configObj, TDF_PROXY_SETTING& proxy_settings)
{
	memset(&proxy_settings, 0, sizeof(proxy_settings));
	proxy_settings.nProxyType = configObj.nProxyType;
	strncpy_s(proxy_settings.szProxyHostIp, configObj.szProxyHostIp, sizeof(proxy_settings.szProxyHostIp)-1);
	_snprintf(proxy_settings.szProxyPort, sizeof(proxy_settings.szProxyPort)-1, "%d", configObj.nProxyHostPort);
	strncpy_s(proxy_settings.szProxyUser, configObj.szProxyUser, sizeof(proxy_settings.szProxyUser)-1);
	strncpy_s(proxy_settings.szProxyPwd, configObj.szProxyPassword, sizeof(proxy_settings.szProxyPwd)-1);
}

char* NQ::MarketQueryRequest::DeepCopy(const char*szIn)
{
	if (szIn)
	{
		int nLen = strlen(szIn);
		char* pRet = new char[nLen+1];
		pRet[nLen] = 0;
		strncpy_s(pRet, nLen+1, szIn, nLen+1);
		return pRet;
	}
	else
	{
		return NULL;
	}
}

void NQ::MarketQueryRequest::RecvData(THANDLE hTdf, TDF_MSG* pMsgHead)
{
	if (!pMsgHead->pData)
	{
		assert(0);
		return ;
	}

	unsigned int nItemCount = pMsgHead->pAppHead->nItemCount;
	unsigned int nItemSize = pMsgHead->pAppHead->nItemSize;

	if (!nItemCount)
	{
		assert(0);
		return ;
	}
	static __time64_t nLastLogTick = 0;
	switch(pMsgHead->nDataType)
	{
	case MSG_DATA_MARKET:
		{
			assert(nItemSize == sizeof(TDF_MARKET_DATA));

			static __time64_t nLastUpdateTime = 0;

			//g_marketReq->writeLog("获取市场行情" + std::to_string(nItemCount) + "条记录");
			//循环处理返回行情
			for (int i = 0; i < nItemCount; i++)
			{
				TDF_MARKET_DATA* pLastMarket = GETRECORD(pMsgHead->pData,TDF_MARKET_DATA, i);
				std::string szWindCode = std::string(pLastMarket->szWindCode);
				if (m_marketData.find(szWindCode)!=m_marketData.end()){
					if (!dataModified(*pLastMarket)) {
						continue;
					}
				}else
				{
					m_marketData.insert(std::map<std::string, TDF_MARKET_DATA>::value_type(szWindCode, *pLastMarket));
				}
				NQ_ET::SQuote tickData = getTickData(*pLastMarket);
				while (g_marketReq->m_threadCnt > g_marketReq->max_threadCnt)
				{
					Sleep(100);
				}
				CLock cl(&m_cs);
				g_marketReq->m_threadCnt++;
				if (g_marketReq)
				{
					std::thread tmpThread(&NQ::MarketQueryRequest::callBackTick, g_marketReq, tickData);
					tmpThread.detach();
				}
			}
		}
		break;
	case MSG_DATA_INDEX:
		{
			assert(nItemSize == sizeof(TDF_INDEX_DATA));
			
			//g_marketReq->writeLog("获取指数行情" + std::to_string(nItemCount) + "条记录");
			for (int i = 0; i < nItemCount; i++)
			{
				TDF_INDEX_DATA* pLastIndex = GETRECORD(pMsgHead->pData,TDF_INDEX_DATA, i);
				std::string szWindCode = std::string(pLastIndex->szWindCode);
				/*
				if (m_indexData.find(szWindCode)!= m_indexData.end()){
					m_indexData.erase(m_indexData.find(szWindCode));
				}
				*/
				if (m_indexData.find(szWindCode)!= m_indexData.end()){
					if (!dataModified(*pLastIndex)) {
						continue;
					}
				}else
				{
					m_indexData.insert(std::map<std::string, TDF_INDEX_DATA>::value_type(szWindCode, *pLastIndex));
				}
				
				NQ_ET::SQuote tickData = getTickData(*pLastIndex);
				while (g_marketReq->m_threadCnt > g_marketReq->max_threadCnt)
				{
					Sleep(100);
				}
				CLock cl(&m_cs);
				g_marketReq->m_threadCnt++;
				if (g_marketReq)
				{
					std::thread tmpThread(&NQ::MarketQueryRequest::callBackTick, g_marketReq, tickData);
					tmpThread.detach();
				}
			}
		}
	default:
		{
			//assert(0);
		}
		break;
	}
}

NQ_ET::EListedExchange NQ::MarketQueryRequest::findMarket(std::string market)
{
	NQ_ET::EListedExchange exchangeCode;
	if (market=="SZ")
	{
		exchangeCode = NQ_ET::EListedExchange::SZSE;
	} else if (market=="SH")
	{
		exchangeCode = NQ_ET::EListedExchange::SHSE;
	} else if (market=="CF")
	{
		exchangeCode = NQ_ET::EListedExchange::CFE;
	} else if (market=="CHF")
	{
		exchangeCode = NQ_ET::EListedExchange::SHFE;
	} else if (market=="CZC")
	{
		exchangeCode = NQ_ET::EListedExchange::ZCE;
	} else if (market=="DCE")
	{
		exchangeCode = NQ_ET::EListedExchange::DCE;
	}
	return exchangeCode;
}

NQ_ET::SQuote NQ::MarketQueryRequest::getTickData(TDF_INDEX_DATA& data)
{
	NQ_ET::SQuote dTick;
	std::string szWindCode = data.szWindCode;
	int dotPos = szWindCode.find('.');
	std::string market = szWindCode.substr(dotPos+1,szWindCode.length()-dotPos-1);
	NQ_ET::EListedExchange exchangeCode = findMarket(market);
	
	dTick.StkCode = data.szCode;
	dTick.StkName = "";
	dTick.CurrentDate = data.nActionDay;
	dTick.CurrentTime = data.nTime;
	dTick.PreClose = (1.0*data.nPreCloseIndex)/10000;
	dTick.TodayOpen = (1.0*data.nOpenIndex) / 10000;
	dTick.TodayHigh = (1.0*data.nHighIndex) / 10000;
	dTick.TodayLow = (1.0*data.nLowIndex) / 10000;
	dTick.LastPrice = (1.0*data.nLastIndex) / 10000;
	dTick.WVAP = (1.0*data.iTurnover)/data.iTotalVolume/10000;
	dTick.Volume = data.iTotalVolume;
	dTick.ExchangeCode = exchangeCode;
	
	//置空
	dTick.Status = 0;
	dTick.UpperLimitPrice = 0;
	dTick.LowerLimitPrice = 0;
	unsigned int tmp[10] = {0,0,0,0,0,0,0,0,0,0};
	Util::intArrayToDouble(tmp, dTick.BidPrice, 10, 10000);
	Util::intArrayToDouble(tmp, dTick.BidVol, 10, 10000);
	Util::intArrayToDouble(tmp, dTick.AskPrice, 10, 10000);
	Util::intArrayToDouble(tmp, dTick.AskVol, 10, 10000);

	return dTick;


}

NQ_ET::SQuote NQ::MarketQueryRequest::getTickData(TDF_MARKET_DATA& data)
{
	std::string szWindCode = data.szWindCode;
	std::map<std::string, std::string>::iterator it = m_marketCode.find(szWindCode);
	std::string name = "";
	if ( it != m_marketCode.end()){
		name = it->second;
	} 

	int dotPos = szWindCode.find('.');
	std::string market = szWindCode.substr(dotPos+1,szWindCode.length()-dotPos-1);
	NQ_ET::EListedExchange exchangeCode = findMarket(market);

	NQ_ET::SQuote dTick;

	dTick.StkCode = data.szCode;
	dTick.StkName =	name;
	dTick.CurrentDate = data.nActionDay;
	dTick.CurrentTime =	data.nTime;
	dTick.Status = data.nStatus;
	dTick.PreClose = (data.nPreClose*1.0)/10000;
	dTick.TodayOpen = (data.nOpen*1.0)/10000;
	dTick.TodayHigh = (data.nHigh*1.0)/10000;
	dTick.TodayLow = (data.nLow*1.0)/10000;
	dTick.LastPrice = (data.nMatch*1.0)/10000;
	dTick.WVAP = (data.iTurnover*1.0)/data.iVolume/10000;
	dTick.UpperLimitPrice = (data.nHighLimited*1.0)/10000;
	dTick.LowerLimitPrice = (data.nLowLimited*1.0)/10000;
	dTick.Volume = data.nNumTrades;
	Util::intArrayToDouble(data.nBidPrice, dTick.BidPrice, 10, 10000);
	Util::intArrayToDouble(data.nBidVol, dTick.BidVol, 10, 10000);
	Util::intArrayToDouble(data.nAskPrice, dTick.AskPrice, 10, 10000);
	Util::intArrayToDouble(data.nAskVol, dTick.AskVol, 10, 10000);
	dTick.ExchangeCode = exchangeCode;

	return dTick;
}

void NQ::MarketQueryRequest::RecvSys(THANDLE hTdf, TDF_MSG* pSysMsg)
{
	if (!pSysMsg ||! hTdf)
	{
		return;
	}

	switch (pSysMsg->nDataType)
	{
	case MSG_SYS_DISCONNECT_NETWORK:
		g_marketReq->writeLog("断开连接");
		//g_marketReq->g_hTDF = NULL;
		//g_marketReq->getConnection();
		break;
	case MSG_SYS_CONNECT_RESULT:
		{
			//g_marketReq->g_hTDF = hTdf;
			TDF_CONNECT_RESULT* pConnResult = (TDF_CONNECT_RESULT*)pSysMsg->pData;
			if (pConnResult && pConnResult->nConnResult)
			{
				g_marketReq->writeLog("连接成功!"); 
				g_marketReq->writeLog("ip:         " + std::string(pConnResult->szIp));
				g_marketReq->writeLog("port:       " + std::string(pConnResult->szPort));
				g_marketReq->writeLog("user:       " + std::string(pConnResult->szUser));
				g_marketReq->writeLog("password:   " + std::string(pConnResult->szPwd));
			}
			else
			{
				g_marketReq->writeLog("连接失败!"); 
				g_marketReq->writeLog("ip:         " + std::string(pConnResult->szIp));
				g_marketReq->writeLog("port:       " + std::string(pConnResult->szPort));
				g_marketReq->writeLog("user:       " + std::string(pConnResult->szUser));
				g_marketReq->writeLog("password:   " + std::string(pConnResult->szPwd));
			}
		}
		break;
	case MSG_SYS_LOGIN_RESULT:
		{
			//g_marketReq->writeLog("登录成功");
			//g_marketReq->g_hTDF = hTdf;
			TDF_LOGIN_RESULT* pLoginResult = (TDF_LOGIN_RESULT*)pSysMsg->pData;
			if (pLoginResult && pLoginResult->nLoginResult)
			{
				g_marketReq->writeLog("登陆成功!");
				g_marketReq->writeLog("登陆信息: " + std::string(pLoginResult->szInfo));
				for (int i=0; i<pLoginResult->nMarkets; i++)
				{ 
					g_marketReq->writeLog("market:   " + std::string(pLoginResult->szMarket[i])); 
					g_marketReq->writeLog("dyn_date: " + std::to_string(pLoginResult->nDynDate[i]));
				}
			}
			else
			{
				g_marketReq->writeLog("登陆失败，原因: " 
					+ std::string(pLoginResult->szInfo));
			}
		}
		break;
	case MSG_SYS_QUOTATIONDATE_CHANGE:
		break;
	case MSG_SYS_MARKET_CLOSE:
		g_marketReq->writeLog("闭市");
		g_marketReq->saveTodayMarket();
		break;
	case MSG_SYS_HEART_BEAT:
		break;
	case MSG_SYS_CODETABLE_RESULT:
		{
			TDF_CODE_RESULT* pCodeResult = (TDF_CODE_RESULT*)pSysMsg->pData;
			//g_marketReq->writeLog("获取" + std::to_string(pCodeResult->nMarkets) + "个市场的代码表");
			if (pCodeResult )
			{
				for (int i=0; i<pCodeResult->nMarkets; i++)
				{
					//获取代码表 
					TDF_CODE* pCodeTable; 
					unsigned int nItems;
					//if (pCodeResult->szMarket[i] == "SZ" || pCodeResult->szMarket[i] == "SH"){
					TDF_GetCodeTable(hTdf, pCodeResult->szMarket[i], &pCodeTable, &nItems);
					//}
					for (int i=0; i<nItems; i++)
					{
						TDF_CODE code = TDF_CODE(pCodeTable[i]);
						if (m_marketCode.find(std::string(code.szWindCode))==m_marketCode.end())
						{
							m_marketCode.insert(std::map<std::string, std::string>::value_type
								(std::string(code.szWindCode), code.szCNName));
						}
					}
					TDF_FreeArr(pCodeTable);

				}
			}
		}
		break;
	default:
		assert(0);
		break;
	}
}

void NQ::MarketQueryRequest::saveTodayMarket()
{
	g_marketReq->writeLog("写入每日收盘行情文件");
	g_marketReq->writeLog("共有" + std::to_string(m_marketData.size()) + "条市场行情记录");
	char today[32]  ="";
	time_t time = std::time(NULL);
	tm temptm;
	localtime_s(&temptm, &time);
	sprintf_s(today,"%d年%02d月%02d日",temptm.tm_year+1900,temptm.tm_mon+1,temptm.tm_mday);  
	std::string strDataPath(cfgSettings.szDataDir, 0, sizeof(cfgSettings.szDataDir)-1);
	strDataPath += std::string(today);
	strDataPath += "行情数据文件.txt";

	std::ofstream t_data = initDataFile(strDataPath, true);
	t_data.setf(std::ios::fixed, std::ios::floatfield);
	//g_fsData.open(strDataPath.c_str(), std::ios_base::out);
	if (!t_data.is_open())
	{
		throw OpenFileFailError(strDataPath.c_str());
		return;
	}
	if (m_marketData.size() > 0)
	{
		for (std::map<std::string, TDF_MARKET_DATA>::iterator it = m_marketData.begin(); it != m_marketData.end(); ++it)
		{
			t_data << it->second.szCode << "," 
				<< (it->second.nPreClose * 1.0) / 10000 << ","
				<< (it->second.nOpen * 1.0) / 10000 << ","
				<< (it->second.nHigh * 1.0) / 10000 << ","
				<< (it->second.nLow * 1.0) / 10000 << ","
				<< (it->second.nHighLimited * 1.0) / 10000 << ","
				<< (it->second.nLowLimited * 1.0) / 10000 << ","
				<< (it->second.nMatch * 1.0) / 10000 << ","
				<< it->second.nNumTrades << ","
				<< it->second.iVolume << ","
				<< (it->second.iTurnover * 1.0) / 10000 << std::endl;
		}
	}
	g_marketReq->writeLog("共有" + std::to_string(m_indexData.size()) + "条指数行情记录");
	if (m_indexData.size() > 0)
	{
		for (std::map<std::string, TDF_INDEX_DATA>::iterator it = m_indexData.begin(); it != m_indexData.end(); ++it)
		{
			t_data << it->second.szCode << "," 
				<< (it->second.nPreCloseIndex * 1.0) / 10000 << ","
				<< (it->second.nOpenIndex * 1.0) / 10000 << ","
				<< (it->second.nHighIndex * 1.0) / 10000 << ","
				<< (it->second.nLowIndex * 1.0) / 10000 << ","
				<< ","
				<< ","
				<< ","
				<< ","
				<< it->second.iTotalVolume << ","
				<< (it->second.iTurnover * 1.0) / 10000 << std::endl;
		}
	}
	t_data.flush();
	t_data.close();
}

bool NQ::MarketQueryRequest::dataModified(TDF_MARKET_DATA recentData){
	bool flag = false;
	TDF_MARKET_DATA& preData = m_marketData.find(std::string(recentData.szWindCode))->second;

	Util::ifNotCmpThenCpy(recentData.szCode, preData.szCode, flag, 31);
	Util::ifNotCmpThenCpy(recentData.nStatus, preData.nStatus, flag);
	Util::ifNotCmpThenCpy(recentData.nPreClose, preData.nPreClose, flag);
	Util::ifNotCmpThenCpy(recentData.nOpen, preData.nOpen, flag);
	Util::ifNotCmpThenCpy(recentData.nHigh, preData.nHigh, flag);
	Util::ifNotCmpThenCpy(recentData.nLow, preData.nLow, flag);
	Util::ifNotCmpThenCpy(recentData.nMatch, preData.nMatch, flag);
	Util::ifNotCmpThenCpy(recentData.nAskPrice, preData.nAskPrice, flag, 10);
	Util::ifNotCmpThenCpy(recentData.nAskVol, preData.nAskVol, flag, 10);
	Util::ifNotCmpThenCpy(recentData.nBidPrice, preData.nBidPrice, flag, 10);
	Util::ifNotCmpThenCpy(recentData.nBidVol, preData.nBidVol, flag, 10);
	Util::ifNotCmpThenCpy(recentData.nNumTrades, preData.nNumTrades, flag);
	Util::ifNotCmpThenCpy(recentData.iVolume, preData.iVolume, flag);
	Util::ifNotCmpThenCpy(recentData.iTurnover, preData.iTurnover, flag);
	Util::ifNotCmpThenCpy(recentData.nHighLimited, preData.nHighLimited, flag);
	Util::ifNotCmpThenCpy(recentData.nLowLimited, preData.nLowLimited, flag);

	return flag;
}

bool NQ::MarketQueryRequest::dataModified(TDF_INDEX_DATA recentData){
	bool flag = false;
	TDF_INDEX_DATA& preData = m_indexData.find(std::string(recentData.szWindCode))->second;

	Util::ifNotCmpThenCpy(recentData.szCode, preData.szCode, flag, 31);
	Util::ifNotCmpThenCpy(recentData.nOpenIndex, preData.nOpenIndex, flag);
	Util::ifNotCmpThenCpy(recentData.nHighIndex, preData.nHighIndex, flag);
	Util::ifNotCmpThenCpy(recentData.nLowIndex, preData.nLowIndex, flag);
	Util::ifNotCmpThenCpy(recentData.nLastIndex, preData.nLastIndex, flag);
	Util::ifNotCmpThenCpy(recentData.iTotalVolume, preData.iTotalVolume, flag);
	Util::ifNotCmpThenCpy(recentData.iTurnover, preData.iTurnover, flag);
	Util::ifNotCmpThenCpy(recentData.nPreCloseIndex, preData.nPreCloseIndex, flag);

	return flag;
}

void NQ::MarketQueryRequest::callBackTick(NQ_ET::SQuote dTick)
{
	/*
	std::map<std::string, TDF_MARKET_DATA>::iterator it = m_marketData.find(szWindCode);
	TDF_MARKET_DATA data = it->second;
	std::string name = m_marketCode.find(szWindCode)->second;
	int dotPos = szWindCode.find('.');
	std::string market = szWindCode.substr(dotPos+1,szWindCode.length()-dotPos-1);
	NQ_ET::EListedExchange exchangeCode;
	if (market=="SZ")
	{
	exchangeCode = NQ_ET::EListedExchange::SZSE;
	} else if (market=="SH")
	{
	exchangeCode = NQ_ET::EListedExchange::SHSE;
	} else if (market=="CF")
	{
	exchangeCode = NQ_ET::EListedExchange::CFE;
	} else if (market=="CHF")
	{
	exchangeCode = NQ_ET::EListedExchange::SHFE;
	} else if (market=="CZC")
	{
	exchangeCode = NQ_ET::EListedExchange::ZCE;
	} else if (market=="DCE")
	{
	exchangeCode = NQ_ET::EListedExchange::DCE;
	}

	NQ_ET::SQuote dTick;

	dTick.StkCode = data.szCode;
	dTick.StkName =	name;
	dTick.CurrentDate = data.nActionDay;
	dTick.CurrentTime =	data.nTime;
	dTick.Status = data.nStatus;
	dTick.PreClose = (data.nPreClose*1.0)/10000;
	dTick.TodayOpen = (data.nOpen*1.0)/10000;
	dTick.TodayHigh = (data.nHigh*1.0)/10000;
	dTick.TodayLow = (data.nLow*1.0)/10000;
	dTick.LastPrice = (data.nMatch*1.0)/10000;
	dTick.WVAP = (data.iTurnover*1.0)/data.iVolume/10000;
	dTick.UpperLimitPrice = (data.nHighLimited*1.0)/10000;
	dTick.LowerLimitPrice = (data.nLowLimited*1.0)/10000;
	dTick.Volume = data.nNumTrades;
	Util::intArrayToDouble(data.nBidPrice, dTick.BidPrice, 10, 10000);
	Util::intArrayToDouble(data.nBidVol, dTick.BidVol, 10, 10000);
	Util::intArrayToDouble(data.nAskPrice, dTick.AskPrice, 10, 10000);
	Util::intArrayToDouble(data.nAskVol, dTick.AskVol, 10, 10000);
	dTick.ExchangeCode = exchangeCode;
	*/
	//m_marketData.erase(it);
	g_caller->OnData(dTick);

	CLock cl(&m_cs);
	m_threadCnt--;
}

bool NQ::MarketQueryRequest::subscribMarketData(std::string code, SubscribType type){
	std::string szWindCode ;
	if (code != "") {
		szWindCode = getWindCode(code);
		if (szWindCode == "") {
			writeLog("不存在" + code + "的代码");
			return false;
		}
	} else
	{
		szWindCode = "";
	}
	int result = TDF_SetSubscription(g_hTDF, szWindCode.c_str(), SUBSCRIPTION_STYLE(type));
	if (result == TDF_ERR_SUCCESS) return true;
	else return false;
}

bool NQ::MarketQueryRequest::subscribMarketData(std::vector<std::string>& codes, SubscribType type)
{
	std::string szWindCodes;
	std::string tempCode;
	std::vector<std::string>::iterator it=codes.begin();
	//20150630 modified
	while(it != codes.end())
	{
		tempCode = getWindCode(*it);
		szWindCodes += tempCode;
		it = codes.erase(it);
		if (it != codes.end() && szWindCodes.length() < 1024){
			if (tempCode != "")	szWindCodes += ";";
		} else
		{
			int result = TDF_SetSubscription(g_hTDF, szWindCodes.c_str(), SUBSCRIPTION_STYLE(type));
			if (result != TDF_ERR_SUCCESS)
			{
				return false;
			}
			szWindCodes.clear();

		}
	}
	return true;

}

std::string NQ::MarketQueryRequest::getWindCode(std::string code)
{
	for(std::map<std::string, std::string>::iterator it=m_marketCode.begin(); it!=m_marketCode.end(); ++it){
		std::string szWindCode = it->first;
		if (szWindCode.substr(0,szWindCode.find('.')) == code)
		{
			return szWindCode;
		}
	}
	return "";
}



void NQ::MarketQueryRequest::writeLog(std::string content) 
{
	g_fsLog << content << std::endl;
	g_fsLog.flush();
}