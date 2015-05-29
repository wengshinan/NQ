#include "MarketQueryRequest.h"


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

		std::string key = trim(line.substr(0,pos));
		std::string value;
		if(end==std::string::npos) value = trim(line.substr(pos+1));
		else value = pos+1>=end?"":trim(line.substr(pos+1,end-pos-1));

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
		if (it!=key2value.end()) strncpy_s(szDataDir, it->second.c_str(), 31);
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
	}

	key2value.clear();
	return true;
}

NQ::MarketQueryRequest::MarketQueryRequest(std::string strConfigFile){
	bool bLoadSettings = cfgSettings.LoadSettings(strConfigFile);
	if(!bLoadSettings) {
		throw ReadConfigFailError(strConfigFile);
		return;
	}

	std::string strLogPath(cfgSettings.szDataDir, 0, sizeof(cfgSettings.szDataDir)-1);
	strLogPath += "日志文件.txt";
	g_fsLog.open(strLogPath.c_str(), std::ios_base::out);
	if (!g_fsLog.is_open())
	{
		throw OpenFileFailError(strLogPath.c_str());
		return;
	}

	g_hTDF = NULL;
	g_marketReq = this;
}

NQ::MarketQueryRequest::~MarketQueryRequest(){
	m_marketCode.clear();
	m_marketData.clear();
}

void NQ::MarketQueryRequest::setEnvironment()
{

	TDF_SetEnv(TDF_ENVIRON_HEART_BEAT_INTERVAL, cfgSettings.nHeartBeatGap);
	TDF_SetEnv(TDF_ENVIRON_MISSED_BEART_COUNT, cfgSettings.nMissedHeartBeatCount);
	TDF_SetEnv(TDF_ENVIRON_OPEN_TIME_OUT, cfgSettings.nOpenTimeOut);

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
			delete[] open_settings.szMarkets;
			delete[] open_settings.szSubScriptions;
			throw ConnectFailError();
			return;
		}

		delete[] open_settings.szMarkets;
		delete[] open_settings.szSubScriptions;
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

			// TODO
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
				std::thread tmpThread(&NQ::MarketQueryRequest::callBackTick, g_marketReq, szWindCode);
				tmpThread.detach();
			}

		}
		break;
	default:
		{
			assert(0);
		}
		break;
	}
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
		break;
	case MSG_SYS_CONNECT_RESULT:
		break;
	case MSG_SYS_LOGIN_RESULT:
		break;
	case MSG_SYS_QUOTATIONDATE_CHANGE:
		break;
	case MSG_SYS_MARKET_CLOSE:
		break;
	case MSG_SYS_HEART_BEAT:
		break;
	case MSG_SYS_CODETABLE_RESULT:
		{
			TDF_CODE_RESULT* pCodeResult = (TDF_CODE_RESULT*)pSysMsg->pData;
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
						TDF_CODE& code = pCodeTable[i];
						if (m_marketCode.find(std::string(code.szWindCode))==m_marketCode.end())
						{
							m_marketCode.insert(std::map<std::string, TDF_CODE>::value_type
								(std::string(code.szWindCode), code));
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


bool NQ::MarketQueryRequest::dataModified(TDF_MARKET_DATA recentData){
	bool flag = false;
	TDF_MARKET_DATA& preData = m_marketData.find(std::string(recentData.szWindCode))->second;

	ifNotCmpThenCpy(recentData.szCode, preData.szCode, flag, 31);
	ifNotCmpThenCpy(recentData.nStatus, preData.nStatus, flag);
	ifNotCmpThenCpy(recentData.nPreClose, preData.nPreClose, flag);
	ifNotCmpThenCpy(recentData.nOpen, preData.nOpen, flag);
	ifNotCmpThenCpy(recentData.nHigh, preData.nHigh, flag);
	ifNotCmpThenCpy(recentData.nLow, preData.nLow, flag);
	ifNotCmpThenCpy(recentData.nMatch, preData.nMatch, flag);
	ifNotCmpThenCpy(recentData.nAskPrice, preData.nAskPrice, flag, 10);
	ifNotCmpThenCpy(recentData.nAskVol, preData.nAskVol, flag, 10);
	ifNotCmpThenCpy(recentData.nBidPrice, preData.nBidPrice, flag, 10);
	ifNotCmpThenCpy(recentData.nBidVol, preData.nBidVol, flag, 10);
	ifNotCmpThenCpy(recentData.nNumTrades, preData.nNumTrades, flag);
	ifNotCmpThenCpy(recentData.iVolume, preData.iVolume, flag);
	ifNotCmpThenCpy(recentData.iTurnover, preData.iTurnover, flag);
	ifNotCmpThenCpy(recentData.nHighLimited, preData.nHighLimited, flag);
	ifNotCmpThenCpy(recentData.nLowLimited, preData.nLowLimited, flag);

	return flag;
}

void NQ::MarketQueryRequest::callBackTick(std::string szWindCode)
{
	TDF_MARKET_DATA data = m_marketData.find(szWindCode)->second;
	TDF_CODE code = m_marketCode.find(szWindCode)->second;
	std::string name = code.szCNName;
	char* market = code.szMarket;
	NQ_ET::EListedExchange exchangeCode;
	if (strcmp(market,"SZ"))
	{
		exchangeCode = NQ_ET::EListedExchange::SZSE;
	} else if (strcmp(market,"SH"))
	{
		exchangeCode = NQ_ET::EListedExchange::SHSE;
	} else if (strcmp(market,"CF"))
	{
		exchangeCode = NQ_ET::EListedExchange::CFE;
	} else if (strcmp(market,"CHF"))
	{
		exchangeCode = NQ_ET::EListedExchange::SHFE;
	} else if (strcmp(market,"CZC"))
	{
		exchangeCode = NQ_ET::EListedExchange::ZCE;
	} else if (strcmp(market,"DCE"))
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
	intArrayToDouble(data.nBidPrice, dTick.BidPrice, 10, 10000);
	intArrayToDouble(data.nBidVol, dTick.BidVol, 10, 10000);
	intArrayToDouble(data.nAskPrice, dTick.AskPrice, 10, 10000);
	intArrayToDouble(data.nAskVol, dTick.AskVol, 10, 10000);
	dTick.ExchangeCode = exchangeCode;

	this->caller->onRespMarketQuery(dTick);
}

bool NQ::MarketQueryRequest::subscribMarketData(std::string code, SubscribType type){
	std::string szWindCode = getWindCode(code);
	int result = TDF_SetSubscription(g_hTDF, szWindCode.c_str(), SUBSCRIPTION_STYLE(type));
	if (result == TDF_ERR_SUCCESS) return true;
	else return false;
}

bool NQ::MarketQueryRequest::subscribMarketData(std::vector<std::string>& codes, SubscribType type)
{
	std::string szWindCodes;
	std::vector<std::string>::iterator it=codes.begin();
	while(true)
	{
		szWindCodes += *it;
		if (codes.erase(it)!=codes.end() && szWindCodes.length() < 1024){
			szWindCodes += ";";
		} else
		{
			int result = TDF_SetSubscription(g_hTDF, szWindCodes.c_str(), SUBSCRIPTION_STYLE(type));
			while (result != TDF_ERR_SUCCESS)
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
	for(std::map<std::string, TDF_CODE>::iterator it=m_marketCode.begin(); it!=m_marketCode.end(); ++it){
		if (it->second.szCode == code)
		{
			return it->second.szWindCode;
		}
	}
	return NULL;
}