#ifndef MARKET_INTERFACE_H
#define MARKET_INTERFACE_H


#include <string>
#include "TradeCenter.h"
#include <vector>
#include "TDFAPIStruct.h"
#include "TDFAPI.h"
#include <assert.h>

namespace NQ_ET{
	//�ֲַ���
	enum EPosDir
	{
		PosLONG, PosSHORT, PosCLEAR
	};

	//���׷���
	enum ETradeDir
	{
		TDBUY,TDSELL,TDUNKNOWN
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

	//Tick�ṹ�壨�������ڹ�ƱLv2ʮ�����飩
	struct SQuote
	{
		//��Ʊ����
		std::string StkCode;
		//��˾����
		std::string StkName;
		//���ڣ���Ϊ����ʱ�䣩
		int	CurrentDate;
		//ʱ��
		int CurrentTime;
		//����״̬��������������������ʱ�Ρ����Ͼ��ۺͷǽ���ʱ�Σ�
		int Status;
		//ǰ���̼�
		double PreClose;
		//���տ��̼�
		double TodayOpen;
		//������߼�
		double TodayHigh;
		//������ͼ�
		double TodayLow;
		//��ǰ�۸�
		double LastPrice;
		//�ɽ�����
		double WVAP;
		//��ͣ��۸�
		double UpperLimitPrice;
		//��ͣ��۸�
		double LowerLimitPrice;
		//��ǰ�ɽ������ɽ�������
		int Volume;
		//�����
		double BidPrice[10];
		//������
		double BidVol[10];
		//������
		double AskPrice[10];
		//������
		double AskVol[10];
		//������
		EListedExchange ExchangeCode;
	};

	//��ʳɽ����������ڹ�ƱLv2��ʳɽ���
	struct STransaction
	{
		//��Ʊ����
		std::string StkCode;
		//��˾����
		std::string StkName;
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
		//��˾����
		std::string StkName;
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
		//��˾����
		std::string StkName;
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
		std::string StkCode;
		//��˾����
		std::string StkName;
		//���̼�
		double Oppr;
		//��߼�
		double Hipr;
		//��ͼ�
		double Lopr;
		//���̼�
		double Clpr;
		//�ɽ���
		int Volume;
		//Ƶ��
		int BarSize;
	};

	//�����źŽṹ��
	struct Signal
	{
		//��������
		std::string Strat;
		//�ֲַ���
		EPosDir D;
		//�ֲ�����
		int Shares;
		//���ɽ��۸�
		double LastTradePrice;
		//�ɽ�����
		int ExecShares;
		//����µ����
		NQ::OrderStatus LastOrderStatus;

		//���캯��
		Signal(std::string s){ Strat = s; }
		Signal(){}
	};

	//�������ģ��
	template<class T>
	class IDataInStream
	{
	public:
		virtual bool OnData(T& data) = 0;
	};

	//�����ź����
	class IPositioner
	{
	public:
		virtual	void OnReqUpdateStratPosition(Signal s) = 0;
	};

	//���ײ���ģ��
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
		//�������飬ֻ��Ҫִ��һ�Σ������سɹ������ĳɹ�
		virtual int init(std::string marketConfigFile);
		//�ֶ���������
		virtual int reqMarketQuery();
		//�ص�
		virtual int onRespMarketQuery(NQ_ET::SQuote tickData)=0;
	};
}

#endif // !MARKET_INTERFACE_H