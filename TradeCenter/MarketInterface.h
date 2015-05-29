#ifndef MARKET_INTERFACE_H
#define MARKET_INTERFACE_H


#include <string>
#include "TradeCenter.h"
#include <vector>
#include "TDFAPIStruct.h"
#include "TDFAPI.h"
#include <assert.h>

namespace NQ_ET{
	//持仓方向
	enum EPosDir
	{
		PosLONG, PosSHORT, PosCLEAR
	};

	//交易方向
	enum ETradeDir
	{
		TDBUY,TDSELL,TDUNKNOWN
	};

	//交易所
	enum EListedExchange
	{
		SHSE, //上海证券交易所
		SZSE, //深圳证券交易所
		HKSE, //香港证券交易所
		SHFE, //上海期货交易所
		CFE,  //中国金融期货交易所
		DCE,  //大连商品交易所
		ZCE   //郑州商品交易所
	};

	//Tick结构体（仅适用于股票Lv2十档行情）
	struct SQuote
	{
		//股票代码
		std::string StkCode;
		//公司名称
		std::string StkName;
		//日期（可为本机时间）
		int	CurrentDate;
		//时间
		int CurrentTime;
		//最新状态（需至少区分正常交易时段、集合竞价和非交易时段）
		int Status;
		//前收盘价
		double PreClose;
		//今日开盘价
		double TodayOpen;
		//今日最高价
		double TodayHigh;
		//今日最低价
		double TodayLow;
		//当前价格
		double LastPrice;
		//成交均价
		double WVAP;
		//涨停板价格
		double UpperLimitPrice;
		//跌停板价格
		double LowerLimitPrice;
		//当前成交量（成交笔数）
		int Volume;
		//申买价
		double BidPrice[10];
		//申买量
		double BidVol[10];
		//申卖价
		double AskPrice[10];
		//申卖量
		double AskVol[10];
		//交易所
		EListedExchange ExchangeCode;
	};

	//逐笔成交（仅适用于股票Lv2逐笔成交）
	struct STransaction
	{
		//股票代码
		std::string StkCode;
		//公司名称
		std::string StkName;
		//日期（可为本机时间）
		int	CurrentDate;
		//时间
		int CurrentTime;
		//交易所
		EListedExchange ExchangeCode;
		//成交价格
		double Price;
		//成交数量
		int Volume;
		//成交金额
		double Turnover;
		//成交类型（可选）
		char OrderType;
		//成交方向（可选，若无此字段可填TDUNKNOWN）
		ETradeDir TradeDirection;
	};

	//逐笔委托（仅适用于股票Lv2逐笔委托）
	struct SOrderSequence
	{
		//股票代码
		std::string StkCode;
		//公司名称
		std::string StkName;
		//日期（可为本机时间）
		int	CurrentDate;
		//时间
		int CurrentTime;
		//委托价格
		double Price;
		//委托数量
		int Volume;
		//委托类型
		char OrderType;
		//委托交易方向
		ETradeDir TradeDirection;
	};

	//订单队列（仅适用于股票Lv2订单队列）
	struct SQueue
	{
		//股票代码
		std::string StkCode;
		//公司名称
		std::string StkName;
		//日期（可为本机时间）
		int	CurrentDate;
		//时间
		int CurrentTime;
		//交易所
		EListedExchange ExchangeCode;
		//买卖方向
		ETradeDir TradeDirection;
		//订单价格
		double Price;
		//订单数量
		int Volume;
	};

	//Bar结构体
	struct SBar
	{
		//股票代码
		std::string StkCode;
		//公司名称
		std::string StkName;
		//开盘价
		double Oppr;
		//最高价
		double Hipr;
		//最低价
		double Lopr;
		//收盘价
		double Clpr;
		//成交量
		int Volume;
		//频率
		int BarSize;
	};

	//交易信号结构体
	struct Signal
	{
		//策略名称
		std::string Strat;
		//持仓方向
		EPosDir D;
		//持仓数量
		int Shares;
		//最后成交价格
		double LastTradePrice;
		//成交数量
		int ExecShares;
		//最后下单结果
		NQ::OrderStatus LastOrderStatus;

		//构造函数
		Signal(std::string s){ Strat = s; }
		Signal(){}
	};

	//数据输出模板
	template<class T>
	class IDataInStream
	{
	public:
		virtual bool OnData(T& data) = 0;
	};

	//交易信号输出
	class IPositioner
	{
	public:
		virtual	void OnReqUpdateStratPosition(Signal s) = 0;
	};

	//交易策略模板
	class IStrategy
	{
	public:
		virtual void ReqLoad(IPositioner* pPositioner) = 0;
		virtual	void ReqUnload() = 0;
		virtual void OnRespUpdatePosition(Signal s) = 0;

		void AddInput(IPositioner* pPositioner)
		{
			if(pPositioner)
			{
				IPositioners.push_back(pPositioner);
			}
		}
	protected:
		void ReqUpdateStratPosition(Signal s)
		{
			for (std::vector<IPositioner*>::iterator it = IPositioners.begin(); it != IPositioners.end(); ++it)
			{
				IPositioner* pPositioner = *it;
				pPositioner->OnReqUpdateStratPosition(s);
			}
		}

		std::vector<IPositioner*> IPositioners;
	};
}

namespace NQ{
	__interface MarketCaller
	{
	public:
		//请求行情，只需要执行一次，若返回成功，则订阅成功
		virtual int init(std::string marketConfigFile);
		//手动订阅行情
		virtual int reqMarketQuery();
		//回调
		virtual int onRespMarketQuery(NQ_ET::SQuote tickData)=0;
	};
}

#endif // !MARKET_INTERFACE_H