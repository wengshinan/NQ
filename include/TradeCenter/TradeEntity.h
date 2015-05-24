#ifndef TRADE_ENTITY_H
#define TRADE_ENTITY_H

#include "TradeBase.h"
#include <list>

#define AccountNo std::string
#define Password std::string
#define StockCode std::string
#define StockNum double
#define Money double
#define StockPrice Money
#define RetMessage std::string
#define OrderId std::string
#define OriOrderId std::string
#define RespOrderId std::string
#define FundPosReqId std::string
#define FundPosReqType int
#define FundPosRespId std::string
#define QryRsltNum int
#define ShareHolderCode std::string
#define PositionNum	double
#define PosAmount double

namespace NQ{

	//报单状态
	enum OrderStatus{
		OrderNotResponse,				//未回报
		OrderConfirmed,					//报单确认
		OrderRejected,					//报单拒绝
		OrderPartlyExecuted,			//部分成交
		OrderAllExecuted,				//全部成交
		OrderCancelling,				//待撤
		OrderCacelled					//已撤
	};

	//撤单状态
	enum CancelStatus
	{
		CancelNotResponse,				//未回报
		CancelConfirmed,				//待撤
		CancelRejected,					//撤单拒绝
		CancelExecuted					//已撤销
	};

	//回报执行状态
	enum ExecuteType
	{
		PartlyExecuted,					//部分成交
		AllExecuted						//全部成交
	};

	// 请求种类
	enum ReqType
	{
		LimitedOrderType,				//限价委托
		CancelOrderType,				//撤单委托
		OrderQueryType,					//持仓查询
		StockQueryType,					//股份查询
		FundQueryType,					//资金查询
		DefaultReq
	};

	// 回报种类
	enum RespType
	{
		OrderApplyingType,				//报单待确认
		OrderConfirmType,				//报单确认
		OrderRejectType,				//报单拒绝
		OrderExecuteType,				//报单回报
		CancelCommitType,				//撤单确认
		CancelRejectType,				//撤单拒绝
		CancelExecuteType,				//撤单回报
		OrderQueryResultType,			//持仓查询回报
		StockQueryResultType,			//股份查询回报
		FundQueryResultType,			//资金查询回报
		DefaultResp
	};

	//登录实体
	struct UserConfiguration{
		AccountType		type;			//登录方式
		AccountNo		accountNo;		//账号
		Password		password;		//密码
		UserConfiguration(
			AccountType type,
			AccountNo no,
			Password password)
			:type(type),
			password(password),
			accountNo(no){}
		UserConfiguration(){}
	};

	//限价报单实体
	struct LimitedOrder{
	public:
		TradeSide		side;				//买卖方向
		StockCode		stock;				//证券代码
		StockNum		num;				//买卖数量
		StockPrice		price;				//委托价格
		Currency		currency;			//币种
		OpenCloseFlag	openCloseFlag;		//开平仓标志
		CashMargin		cashMargin;			//信用交易开仓平仓，与买卖方向组合双融交易
		MarketCode		market;				//市场代码
		TradeRespBase	response;			//报单情况，应用端发起情况，与报单结果无关
		OrderId			id;					//报单请求号，与回报对应
		LimitedOrder(
			TradeSide side,
			StockCode stock,
			StockNum num,
			StockPrice price,
			Currency currency,
			OpenCloseFlag openCloseFlag,
			CashMargin cashMargin,
			MarketCode market)
			:side(side),
			stock(stock),
			num(num),
			price(price),
			currency(currency),
			openCloseFlag(openCloseFlag),
			cashMargin(cashMargin),
			market(market){}
		LimitedOrder(
			TradeSide side,
			StockCode stock,
			StockNum num,
			StockPrice price,
			Currency currency,
			MarketCode market)
			:side(side),
			stock(stock),
			num(num),
			price(price),
			currency(currency),
			market(market){
				openCloseFlag = TradeSide::Buy == side ? OpenCloseFlag::FlagOpen : OpenCloseFlag::FlagClose;
		}
		LimitedOrder(){}
	};

	//报单回报实体
	struct OrderResponse{
		OrderId			id;					//回报请求号，与报单请求号对应
		OrderStatus		status;				//委托状态
		RetMessage		rejectReason;		//拒绝原因
		OrderResponse(
			OrderId id,
			OrderStatus status,
			RetMessage rejectReason)
			:id(id),
			status(status),
			rejectReason(rejectReason){}
		OrderResponse(
			OrderId id,
			OrderStatus status)
			:id(id),
			status(status),
			rejectReason(""){}
		OrderResponse(){}
	};

	//成交回报实体
	struct ExecuteResponse{
		OrderId			id;					//成交回报请求号，与报单请求号对应
		OrderStatus		status;				//委托状态
		StockNum		num;				//累计成交数量
		StockPrice		price;				//平均成交价
		TradeSide		side;				//买卖方向
		StockCode		stock;				//证券代码
		ExecuteType		lastExecType;		//本次执行结果
		StockPrice		lastExecPrice;		//本次成交极爱
		StockNum		lastExecNum;		//本次成交数量
		StockNum		leavesNum;			//未成交股数
		ExecuteResponse(
			OrderId id,
			OrderStatus status,
			StockNum num,
			StockPrice price,
			TradeSide side,
			StockCode stock,
			ExecuteType lastExecType,
			StockPrice lastExecPrice,
			StockNum lastExecNum,
			StockNum leavesNum)
			:id(id),
			status(status),
			num(num),
			price(price),
			side(side),
			stock(stock),
			lastExecType(lastExecType),
			lastExecPrice(lastExecPrice),
			lastExecNum(lastExecNum),
			leavesNum(leavesNum){}
		ExecuteResponse(){}
	};

	//撤单请求实体
	struct CancelOrder{
		OriOrderId		oriOrderId;			//被撤的委托请求单号，从先前委托记录中获取
		StockNum		num;				//撤单数量
		TradeSide		side;				//买卖方向
		StockCode		stock;				//证券代码
		OrderId			orderId;			//撤单委托单号
		TradeRespBase	response;			//撤单情况，应用端发起情况，与撤单结果无关
		CancelOrder(
			OriOrderId oriOrderId,
			StockNum num,
			TradeSide side,
			StockCode stock)
			:oriOrderId(oriOrderId),
			num(num),
			side(side),
			stock(stock){}
		CancelOrder(){}
	};

	//撤单回报实体
	struct CancelResponse{
		OrderId			orderId;			//撤单委托单号
		OriOrderId		oriOrderId;			//被撤单委托请求单号
		CancelStatus	status;				//撤单委托状态
		RetMessage		rejectReason;		//拒绝原因
		CancelResponse(
			OrderId orderId,
			OriOrderId oriOrderId,
			CancelStatus status,
			RetMessage rejectReason)
			:orderId(orderId),
			oriOrderId(oriOrderId),
			status(status),
			rejectReason(rejectReason){}
		CancelResponse(
			OrderId orderId,
			OriOrderId oriOrderId,
			CancelStatus status)
			:orderId(orderId),
			oriOrderId(oriOrderId),
			status(status),
			rejectReason(""){}
		CancelResponse(){}
	};

	//持仓查询实体
	struct OrderQuery{
		OrderId			orderId;			//持仓查询请求单号，对应限价下单的单号，不能随机生成
		TradeSide		side;				//买卖方向
		StockCode		stock;				//证券代码
		TradeRespBase	response;			//查询请求情况，应用端发起情况，与请求结果无关
		OrderQuery(
			OrderId orderId,
			TradeSide side,
			StockCode stock)
			:side(side),
			stock(stock),
			orderId(orderId){}
		OrderQuery(){}
	};

	//持仓查询结果
	struct OrderQueryResponse{
		RespOrderId		orderId;			//持仓查询应答标识，与持仓查询请求单号、限价下单单号对应
		StockNum		totalSum;			//累计成交数量
		StockPrice		averageExecPrice;	//平均成交价格
		StockNum		orderNum;			//委托总数量
		OrderStatus		status;				//报单状态
		StockPrice		orderPrice;			//委托价格
		TradeSide		side;				//买卖方向
		StockCode		stock;				//证券代码
		StockNum		leavesNum;			//未成交数量
		OrderQueryResponse(
			RespOrderId orderId,
			StockNum totalSum,
			StockPrice averageExecPrice,
			StockNum orderNum,
			OrderStatus status,
			StockPrice orderPrice,
			TradeSide side,
			StockCode stock,
			StockNum leavesNum)
			:orderId(orderId),
			totalSum(totalSum),
			averageExecPrice(averageExecPrice),
			orderNum(orderNum),
			status(status),
			orderPrice(orderPrice),
			side(side),
			stock(stock),
			leavesNum(leavesNum){}
		OrderQueryResponse(){}
	};

	//资金股份查询实体
	struct FundPosQuery{
		FundPosReqId		id;				//资金股份查询请求号
		FundPosReqType		type;			//查询种类，股份0，资金9
		TradeRespBase	response;			//查询请求情况，应用端发起情况，与请求结果无关
		FundPosQuery(
			FundPosReqType type)
			:type(type){}
		FundPosQuery(){}
	};

	struct NoPosition{
		PositionType	type;				//股份类型，股份余额/股份可用余额/当前拥股数等等
		PositionNum		longQty;			//多仓数量
		PositionNum		shortQty;			//空仓数量
		NoPosition(
			PositionType type, 
			PositionNum longQty,
			PositionNum shortQty)
			:type(type),
			longQty(longQty),
			shortQty(shortQty){}
	};

	struct NoPosAmt{
		PosAmtType		type;				//金额类型，资金余额/资金可用余额/资产总额等
		PosAmount		amount;				//金额
		NoPosAmt(
			PosAmtType type,
			PosAmount amount)
			:type(type),
			amount(amount){}
	};

	typedef std::list<NoPosition> Positions;

	typedef std::list<NoPosAmt> Amounts;

	//资金股份查询结果
	struct FundPosQueryResponse{
		FundPosRespId			id;				//资金股份查询请求号，与查询请求对应
		FundPosReqType			type;			//请求类别
		RespCode				response;		//请求状态，成功/失败
		RetMessage				description;	//失败说明
		Currency				currency;		//币种
		StockCode				stock;			//证券代码
		MarketCode				market;			//市场代码
		ShareHolderCode			account;		//股东代码，当请求类别为股份时
		Positions				positions;		//股份数量
		Amounts					amounts;		//资金数量
		FundPosQueryResponse(){}
		FundPosQueryResponse(
			FundPosRespId id,
			FundPosReqType type,
			RespCode response,
			RetMessage description)
			:id(id),
			type(type),
			response(response),
			description(description){}
	};
}

#endif