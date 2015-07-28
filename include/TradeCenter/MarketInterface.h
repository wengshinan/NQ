/////////////////////////////////
// Interface in equity trading vehicle
// NQ_ET is the namespace for Newton Quant Equity Trading System
///////////////////////////////////////
// Coded & designed by R.Cao, PhD, CFA
// All rights reserved by NewtonQuant LLP 2015
//////////////////////////////////////////////

#ifndef EQUITYINTERFACE_H
#define EQUITYINTERFACE_H

#include "TradeCenter.h"
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <map>

namespace NQ_ET{
	//持仓方向
	enum EPosDir
	{
		PosLONG = 1, PosSHORT = -1, PosCLEAR
	};

	//交易方向
	enum ETradeDir
	{
		TDBUY,TDSELL,TDUNKNOWN
	};

	//交易信号类型
	enum ESignalStatus
	{
		SSUninitiated,	//用于未初始化交易信号，撮合器不处理
		SSContigent,	//触发类条件单，顺向类型
		SSPending,		//盈利平仓单类型，仅用于利润锁定
		SSImmediate		//非条件单，既包括需立即成交类型也包括传统限价单
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

	//交易策略类型
	enum EStrat
	{
		TSDAYOFWEEK = 1, 
		TSGAP = 2, 
		TSGSV = 3,
		TSBASIS = 4
	};

	//Tick结构体（仅适用于股票Lv2十档行情）
	struct SQuote
	{
		//股票代码
		StockCode StkCode;
		//股票代码
		StockCode StkName;
		//日期（可为本机时间）
		int	CurrentDate;
		//时间
		int CurrentTime;
		//最新状态（需至少区分正常交易时段、集合竞价和非交易时段）
		int Status;
		//前收盘价
		StockPrice PreClose;
		//今日开盘价
		StockPrice TodayOpen;
		//今日最高价
		StockPrice TodayHigh;
		//今日最低价
		StockPrice TodayLow;
		//当前价格
		StockPrice LastPrice;
		//成交均价
		StockPrice WVAP;
		//涨停板价格
		StockPrice UpperLimitPrice;
		//跌停板价格
		StockPrice LowerLimitPrice;
		//当前成交量（成交笔数）
		StockNum Volume;
		//申买价
		StockPrice BidPrice[10];
		//申买量
		StockNum BidVol[10];
		//申卖价
		StockPrice AskPrice[10];
		//申卖量
		StockNum AskVol[10];
		//交易所
		EListedExchange ExchangeCode;
	};

	//逐笔成交（仅适用于股票Lv2逐笔成交）
	struct STransaction
	{
		//股票代码
		std::string StkCode;
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
		StockCode StkCode;
		//开盘价
		StockPrice Oppr;
		//最高价
		StockPrice Hipr;
		//最低价
		StockPrice Lopr;
		//收盘价
		StockPrice Clpr;
		//成交量
		StockNum Volume;
		//频率
		int BarSize;
	};

	//交易信号结构体
	struct Signal
	{
		//股票代码
		StockCode StkCode;
		//策略名称
		EStrat Strat;
		//持仓方向
		EPosDir D;
		//持仓数量
		StockNum Shares;
		//目标成交价格
		StockPrice TargetPrice;
		//成交数量
		StockNum ExecShares;
		//最后下单结果
		NQ::OrderStatus LastOrderStatus;
		//交易信号的当前状态
		ESignalStatus CurrentSignalStatus;
		//成交均价
		StockPrice AvgTradePrice;
		
		//构造函数
		Signal(EStrat s){ Strat = s; }
		Signal(){}

		//复制构造函数
		Signal(const Signal& s)
		{
			this->StkCode = s.StkCode;
			this->Strat = s.Strat;
			this->D = s.D;
			this->Shares = s.Shares;
			this->TargetPrice = s.TargetPrice;
			this->ExecShares = s.ExecShares;
			this->LastOrderStatus = s.LastOrderStatus;
			this->CurrentSignalStatus = s.CurrentSignalStatus;
			this->AvgTradePrice = s.AvgTradePrice;
		}
	};

	//数据输出模板
	template<class T>
	class IDataInStream
	{
	public:
		virtual bool OnData(T& data) = 0;
	};

	//数据分发模板
	template<class T>
	class IDataOutStream
	{
	public:
		IDataOutStream()
		{
			Index = 0;
		}

		int Register(IDataInStream<T>* pOutput)
		{
			std::lock_guard<std::mutex> l (m_Lock);
			++Index;
			m_pOutputs[Index] = pOutput;
			return Index;
		}

		void Deregister(int i)
		{
			std::lock_guard<std::mutex> l (m_Lock);
			std::map<int, IDataInStream<T>*>::iterator it = m_pOutputs.find(i);

			if(it != m_pOutputs.end())
			{
				m_pOutputs.erase(it);
			}
		}

	protected:
		void Distribute(T& data)
		{
			std::lock_guard<std::mutex> l (m_Lock);
			for(std::map<int, IDataInStream<T>*>::iterator it = m_pOutputs.begin(); it != m_pOutputs.end(); ++it)
			{
				IDataInStream<T>* output = it->second;
				std::thread t(&NQ_ET::IDataInStream<T>::OnData, output, data);
				t.detach();
			}
		}
	private:
		std::map<int, IDataInStream<T>*> m_pOutputs;
		int Index;

		std::mutex m_Lock;
	};

	//交易信号输出
	class IPositioner
	{
	public:
		virtual	void OnReqUpdateStratPosition(Signal& s) = 0;
	};
	
	//交易策略模板
	class IStrategy
	{
	public:
		void ReqLoad(IPositioner* pPositioner)
		{
			if(pPositioner)
			{
				IPositioners.push_back(pPositioner);
			}
		}

		void ReqUnload()
		{
			//卸载所有交易接口
			IPositioners.clear();
		}

		virtual void OnRespUpdatePosition(Signal& s) = 0;

		void AddInput(IPositioner* pPositioner)
		{
			if(pPositioner)
			{
				IPositioners.push_back(pPositioner);
			}
		}	

		EStrat GetStratType(){return ISignal.Strat;}
	protected:
		void ReqUpdateStratPosition(Signal& s)
		{
			for (std::vector<IPositioner*>::iterator it = IPositioners.begin(); it != IPositioners.end(); ++it)
			{
				IPositioner* pPositioner = *it;
				pPositioner->OnReqUpdateStratPosition(s);
			}
		}

		std::vector<IPositioner*> IPositioners;
		Signal ISignal;
	};
}

namespace NQ{
	__interface MarketCaller
	{
	public:
		//请求行情，只需要执行一次，若返回成功，则订阅成功
		virtual int init(std::string marketConfigFile)=0;
		//手动订阅行情
		virtual int reqMarketQuery()=0;
		//回调
		virtual int onRespMarketQuery(NQ_ET::SQuote tickData)=0;
	};
}

#endif