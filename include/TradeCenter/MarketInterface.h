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
	//�ֲַ���
	enum EPosDir
	{
		PosLONG = 1, PosSHORT = -1, PosCLEAR
	};

	//���׷���
	enum ETradeDir
	{
		TDBUY,TDSELL,TDUNKNOWN
	};

	//�����ź�����
	enum ESignalStatus
	{
		SSUninitiated,	//����δ��ʼ�������źţ������������
		SSContigent,	//��������������˳������
		SSPending,		//ӯ��ƽ�ֵ����ͣ���������������
		SSImmediate		//�����������Ȱ����������ɽ�����Ҳ������ͳ�޼۵�
	};

	//������
	enum EListedExchange
	{
		SHSE, //�Ϻ�֤ȯ������
		SZSE, //����֤ȯ������
		HKSE, //���֤ȯ������
		SHFE, //�Ϻ��ڻ�������
		CFE,  //�й������ڻ�������
		DCE,  //������Ʒ������
		ZCE   //֣����Ʒ������
	};

	//���ײ�������
	enum EStrat
	{
		TSDAYOFWEEK = 1, 
		TSGAP = 2, 
		TSGSV = 3,
		TSBASIS = 4
	};

	//Tick�ṹ�壨�������ڹ�ƱLv2ʮ�����飩
	struct SQuote
	{
		//��Ʊ����
		StockCode StkCode;
		//��Ʊ����
		StockCode StkName;
		//���ڣ���Ϊ����ʱ�䣩
		int	CurrentDate;
		//ʱ��
		int CurrentTime;
		//����״̬��������������������ʱ�Ρ����Ͼ��ۺͷǽ���ʱ�Σ�
		int Status;
		//ǰ���̼�
		StockPrice PreClose;
		//���տ��̼�
		StockPrice TodayOpen;
		//������߼�
		StockPrice TodayHigh;
		//������ͼ�
		StockPrice TodayLow;
		//��ǰ�۸�
		StockPrice LastPrice;
		//�ɽ�����
		StockPrice WVAP;
		//��ͣ��۸�
		StockPrice UpperLimitPrice;
		//��ͣ��۸�
		StockPrice LowerLimitPrice;
		//��ǰ�ɽ������ɽ�������
		StockNum Volume;
		//�����
		StockPrice BidPrice[10];
		//������
		StockNum BidVol[10];
		//������
		StockPrice AskPrice[10];
		//������
		StockNum AskVol[10];
		//������
		EListedExchange ExchangeCode;
	};

	//��ʳɽ����������ڹ�ƱLv2��ʳɽ���
	struct STransaction
	{
		//��Ʊ����
		std::string StkCode;
		//���ڣ���Ϊ����ʱ�䣩
		int	CurrentDate;
		//ʱ��
		int CurrentTime;
		//������
		EListedExchange ExchangeCode;
		//�ɽ��۸�
		double Price;
		//�ɽ�����
		int Volume;
		//�ɽ����
		double Turnover;
		//�ɽ����ͣ���ѡ��
		char OrderType;
		//�ɽ����򣨿�ѡ�����޴��ֶο���TDUNKNOWN��
		ETradeDir TradeDirection;
	};

	//���ί�У��������ڹ�ƱLv2���ί�У�
	struct SOrderSequence
	{
		//��Ʊ����
		std::string StkCode;
		//���ڣ���Ϊ����ʱ�䣩
		int	CurrentDate;
		//ʱ��
		int CurrentTime;
		//ί�м۸�
		double Price;
		//ί������
		int Volume;
		//ί������
		char OrderType;
		//ί�н��׷���
		ETradeDir TradeDirection;
	};

	//�������У��������ڹ�ƱLv2�������У�
	struct SQueue
	{
		//��Ʊ����
		std::string StkCode;
		//���ڣ���Ϊ����ʱ�䣩
		int	CurrentDate;
		//ʱ��
		int CurrentTime;
		//������
		EListedExchange ExchangeCode;
		//��������
		ETradeDir TradeDirection;
		//�����۸�
		double Price;
		//��������
		int Volume;
	};

	//Bar�ṹ��
	struct SBar
	{
		//��Ʊ����
		StockCode StkCode;
		//���̼�
		StockPrice Oppr;
		//��߼�
		StockPrice Hipr;
		//��ͼ�
		StockPrice Lopr;
		//���̼�
		StockPrice Clpr;
		//�ɽ���
		StockNum Volume;
		//Ƶ��
		int BarSize;
	};

	//�����źŽṹ��
	struct Signal
	{
		//��Ʊ����
		StockCode StkCode;
		//��������
		EStrat Strat;
		//�ֲַ���
		EPosDir D;
		//�ֲ�����
		StockNum Shares;
		//Ŀ��ɽ��۸�
		StockPrice TargetPrice;
		//�ɽ�����
		StockNum ExecShares;
		//����µ����
		NQ::OrderStatus LastOrderStatus;
		//�����źŵĵ�ǰ״̬
		ESignalStatus CurrentSignalStatus;
		//�ɽ�����
		StockPrice AvgTradePrice;
		
		//���캯��
		Signal(EStrat s){ Strat = s; }
		Signal(){}

		//���ƹ��캯��
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

	//�������ģ��
	template<class T>
	class IDataInStream
	{
	public:
		virtual bool OnData(T& data) = 0;
	};

	//���ݷַ�ģ��
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

	//�����ź����
	class IPositioner
	{
	public:
		virtual	void OnReqUpdateStratPosition(Signal& s) = 0;
	};
	
	//���ײ���ģ��
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
			//ж�����н��׽ӿ�
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
		//�������飬ֻ��Ҫִ��һ�Σ������سɹ������ĳɹ�
		virtual int init(std::string marketConfigFile)=0;
		//�ֶ���������
		virtual int reqMarketQuery()=0;
		//�ص�
		virtual int onRespMarketQuery(NQ_ET::SQuote tickData)=0;
	};
}

#endif