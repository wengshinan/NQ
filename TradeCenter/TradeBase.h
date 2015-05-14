#ifndef TRADE_BASE_H
#define TRADE_BASE_H

#include "openssl/des.h"
#include "openssl/blowfish.h"
#include "quickfix/Values.h"
#include <iostream>
#include <time.h>
#include <string>
#include <sstream>
#include <map>
#include <stdexcept>

#define TradeTime time_t
#define TradeResult std::string
#define UniqueId std::string
#define _snprintf _snprintf_s
#define strcpy(_Dest, _Source) strcpy_s(_Dest, sizeof(_Source), _Source)
#define strcat(_Dest, _Source) strcat_s(_Dest, sizeof(_Source), _Source)

// ���ݼ���(���Ļ�������ԭ�ģ���Կ����������)
int Encrypt_org(char *pszEncrypted,  const char *pszOriginal, const char *pszPublicKey, const char *pszEncryptType);

// ���л����ݣ�ԭ�ģ�ԭ�ĳ��ȣ������������������������ȣ�
char *SerializeData(const char *pszDataSrc, int iSizeSrc, char *pszDataDest, int iSizeDest);

// ����Ψһ��
std::string GenerateUniqueID();

namespace NQ{

	const char	ExecTransType_ORDQRY	='3';			//Ӧ�������3��ί��״̬��ѯ
	const int	PosReqType_FUNDS		= 9;			//�������724����ѯ�ʽ�
	const char	MsgType_UAN[]			= "UAN";		//��Ϣ����35���ʽ�ɷݲ�ѯ
	const char	MsgType_UAP[]			= "UAP";		//��Ϣ����35���ʽ�ɷݲ�ѯ�ر�

	template <typename T>
	class EnumParser
	{
		std::map<std::string, T> enumMap;
	public:
		EnumParser();

		T parseEnum(const std::string &value)
		{
			std::map<std::string, T>::const_iterator iValue = enumMap.find(value);
			if (iValue  == enumMap.end())
				throw std::runtime_error("");
			return iValue->second;
		}

	};
	//��¼��ʽ 96
	enum AccountType
	{
		NormalAccount				='Z',		//��ͨ�ʽ��ʺ�
		CreditAccount				='X',		//�����ʽ��˻�
		CombinedAccount				='T',		//��ͨ�ʽ��˻��������˻�(������ͬһ���ͻ�����)һ���¼
		IndividualOptionsAccount	='G'		//������Ȩ�ʽ��˻�
	};

	//���ܷ�ʽ 98
	enum EncryptMethod
	{
		None		=FIX::EncryptMethod_NONE,			//������
		DesEcb		=FIX::EncryptMethod_DES,			//DES-ECB
		BlowFish	=101								//BlowFish
	};

	//�������� 54
	enum TradeSide{
		Buy = FIX::Side_BUY,					//��
		Sell = FIX::Side_SELL					//��
	};
	static const TradeSide EnumTradeSide[] = {TradeSide::Buy, TradeSide::Sell};

	//���� 15
	enum Currency{
		CNY,									//�����
		USD,									//��Ԫ
		HKD										//�۱�
	};
	static const char* EnumCurrency[] = {"CNY","USD","HKD"};

	//�г����� 207
	enum MarketCode{
		XSHG,									//��
		XSHE									//��
	};
	static const char* EnumMarketCode[] = {"XSHG","XSHE"};

	//���׽������
	enum RespCode{
		SUCC						=0,			//�ɹ�	
		FAIL						=1			//ʧ��
	};

	//����״̬
	struct TradeRespBase{
		TradeRespBase(RespCode respCode, TradeResult errorMessage)
			:respCode(respCode),errorMessage(errorMessage){
				TradeTime ttime;
				time(&ttime);
				tradeTime = ttime;
		}
		TradeRespBase(){}
		RespCode					respCode;
		TradeResult					errorMessage;
		TradeTime					tradeTime;
	};

	//��ƽ�ֱ�־ 77
	enum OpenCloseFlag{
		FlagOpen = FIX::OpenClose_OPEN,
		FlagClose = FIX::OpenClose_CLOSE
	};

	/*����ƽ�� 544
	��������
	54	Side 1
	544	CashMargin	2
	��ȯ����
	54	Side	2
	544	CashMargin	2
	��ȯ��ȯ
	54	Side	1
	544	CashMargin	3
	��ȯ����
	54	Side	2
	544	CashMargin	3
	*/
	enum CashMargin{
		MarginCash = FIX::CashMargin_CASH,			//��ͨ����
		MarginOpen = FIX::CashMargin_MARGIN_OPEN,	//���ý���--����
		MarginClose = FIX::CashMargin_MARGIN_CLOSE	//���ý���--ƽ��
	};

	enum ConnectStatus{
		NotConnect,
		Connecting,
		Connected
	};

	//�ɷ�����
	enum PositionType{
		ShareBalance,					//�ɷ����
		ShareAvailable,					//�ɷݿ������
		ShareQuantity,					//��ǰӵ����
		LastBalance,					//�������
		SoldFrozon,						//����������
		ManualFrozon,					//�˹�������
		BuyQuantity						//������������
	};

	//�������
	enum PosAmtType{
		FundBalance,					//�ʽ����
		FundAvailable,					//�ʽ�������
		MainValue,						//�ʲ���ֵ
		Fund,							//�ʽ��ʲ�
		CurentValue,					//��ֵ
		FundBoughtFrozon,				//�ʽ����붳��
		CurrentCost,					//��ǰ�ɱ�
		SingleMarketValue,				//������ֵ
		ProfitLost,						//ӯ��
		PositionCost,					//�ֲֳɱ�
		BoughtProfitLost				//����ӯ��
	};

}




#endif // !TRADE_BASE_H