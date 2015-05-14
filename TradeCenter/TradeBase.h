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

// 数据加密(密文缓冲区，原文，密钥，加密类型)
int Encrypt_org(char *pszEncrypted,  const char *pszOriginal, const char *pszPublicKey, const char *pszEncryptType);

// 序列化数据（原文，原文长度，输出缓冲区，输出缓冲区长度）
char *SerializeData(const char *pszDataSrc, int iSizeSrc, char *pszDataDest, int iSizeDest);

// 生成唯一串
std::string GenerateUniqueID();

namespace NQ{

	const char	ExecTransType_ORDQRY	='3';			//应答发送类别3，委托状态查询
	const int	PosReqType_FUNDS		= 9;			//请求类别724，查询资金
	const char	MsgType_UAN[]			= "UAN";		//消息类型35，资金股份查询
	const char	MsgType_UAP[]			= "UAP";		//消息类型35，资金股份查询回报

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
	//登录方式 96
	enum AccountType
	{
		NormalAccount				='Z',		//普通资金帐号
		CreditAccount				='X',		//信用资金账户
		CombinedAccount				='T',		//普通资金账户与信用账户(必须是同一个客户代码)一起登录
		IndividualOptionsAccount	='G'		//个股期权资金账户
	};

	//加密方式 98
	enum EncryptMethod
	{
		None		=FIX::EncryptMethod_NONE,			//不加密
		DesEcb		=FIX::EncryptMethod_DES,			//DES-ECB
		BlowFish	=101								//BlowFish
	};

	//买卖方向 54
	enum TradeSide{
		Buy = FIX::Side_BUY,					//买
		Sell = FIX::Side_SELL					//卖
	};
	static const TradeSide EnumTradeSide[] = {TradeSide::Buy, TradeSide::Sell};

	//币种 15
	enum Currency{
		CNY,									//人民币
		USD,									//美元
		HKD										//港币
	};
	static const char* EnumCurrency[] = {"CNY","USD","HKD"};

	//市场代码 207
	enum MarketCode{
		XSHG,									//沪
		XSHE									//深
	};
	static const char* EnumMarketCode[] = {"XSHG","XSHE"};

	//交易结果代码
	enum RespCode{
		SUCC						=0,			//成功	
		FAIL						=1			//失败
	};

	//交易状态
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

	//开平仓标志 77
	enum OpenCloseFlag{
		FlagOpen = FIX::OpenClose_OPEN,
		FlagClose = FIX::OpenClose_CLOSE
	};

	/*开仓平仓 544
	融资买入
	54	Side 1
	544	CashMargin	2
	融券卖出
	54	Side	2
	544	CashMargin	2
	买券还券
	54	Side	1
	544	CashMargin	3
	卖券还款
	54	Side	2
	544	CashMargin	3
	*/
	enum CashMargin{
		MarginCash = FIX::CashMargin_CASH,			//普通交易
		MarginOpen = FIX::CashMargin_MARGIN_OPEN,	//信用交易--开仓
		MarginClose = FIX::CashMargin_MARGIN_CLOSE	//信用交易--平仓
	};

	enum ConnectStatus{
		NotConnect,
		Connecting,
		Connected
	};

	//股份类型
	enum PositionType{
		ShareBalance,					//股份余额
		ShareAvailable,					//股份可用余额
		ShareQuantity,					//当前拥股数
		LastBalance,					//昨日余额
		SoldFrozon,						//卖出冻结数
		ManualFrozon,					//人工冻结数
		BuyQuantity						//今日买入数量
	};

	//金额类型
	enum PosAmtType{
		FundBalance,					//资金余额
		FundAvailable,					//资金可用余额
		MainValue,						//资产总值
		Fund,							//资金资产
		CurentValue,					//市值
		FundBoughtFrozon,				//资金买入冻结
		CurrentCost,					//当前成本
		SingleMarketValue,				//个股市值
		ProfitLost,						//盈亏
		PositionCost,					//持仓成本
		BoughtProfitLost				//买入盈亏
	};

}




#endif // !TRADE_BASE_H