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

	//����״̬
	enum OrderStatus{
		OrderNotResponse,				//δ�ر�
		OrderConfirmed,					//����ȷ��
		OrderRejected,					//�����ܾ�
		OrderPartlyExecuted,			//���ֳɽ�
		OrderAllExecuted,				//ȫ���ɽ�
		OrderCancelling,				//����
		OrderCacelled					//�ѳ�
	};

	//����״̬
	enum CancelStatus
	{
		CancelNotResponse,				//δ�ر�
		CancelConfirmed,				//����
		CancelRejected,					//�����ܾ�
		CancelExecuted					//�ѳ���
	};

	//�ر�ִ��״̬
	enum ExecuteType
	{
		PartlyExecuted,					//���ֳɽ�
		AllExecuted						//ȫ���ɽ�
	};

	// ��������
	enum ReqType
	{
		LimitedOrderType,				//�޼�ί��
		CancelOrderType,				//����ί��
		OrderQueryType,					//�ֲֲ�ѯ
		StockQueryType,					//�ɷݲ�ѯ
		FundQueryType,					//�ʽ��ѯ
		DefaultReq
	};

	// �ر�����
	enum RespType
	{
		OrderApplyingType,				//������ȷ��
		OrderConfirmType,				//����ȷ��
		OrderRejectType,				//�����ܾ�
		OrderExecuteType,				//�����ر�
		CancelCommitType,				//����ȷ��
		CancelRejectType,				//�����ܾ�
		CancelExecuteType,				//�����ر�
		OrderQueryResultType,			//�ֲֲ�ѯ�ر�
		StockQueryResultType,			//�ɷݲ�ѯ�ر�
		FundQueryResultType,			//�ʽ��ѯ�ر�
		DefaultResp
	};

	//��¼ʵ��
	struct UserConfiguration{
		AccountType		type;			//��¼��ʽ
		AccountNo		accountNo;		//�˺�
		Password		password;		//����
		UserConfiguration(
			AccountType type,
			AccountNo no,
			Password password)
			:type(type),
			password(password),
			accountNo(no){}
		UserConfiguration(){}
	};

	//�޼۱���ʵ��
	struct LimitedOrder{
	public:
		TradeSide		side;				//��������
		StockCode		stock;				//֤ȯ����
		StockNum		num;				//��������
		StockPrice		price;				//ί�м۸�
		Currency		currency;			//����
		OpenCloseFlag	openCloseFlag;		//��ƽ�ֱ�־
		CashMargin		cashMargin;			//���ý��׿���ƽ�֣��������������˫�ڽ���
		MarketCode		market;				//�г�����
		TradeRespBase	response;			//���������Ӧ�ö˷���������뱨������޹�
		OrderId			id;					//��������ţ���ر���Ӧ
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

	//�����ر�ʵ��
	struct OrderResponse{
		OrderId			id;					//�ر�����ţ��뱨������Ŷ�Ӧ
		OrderStatus		status;				//ί��״̬
		RetMessage		rejectReason;		//�ܾ�ԭ��
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

	//�ɽ��ر�ʵ��
	struct ExecuteResponse{
		OrderId			id;					//�ɽ��ر�����ţ��뱨������Ŷ�Ӧ
		OrderStatus		status;				//ί��״̬
		StockNum		num;				//�ۼƳɽ�����
		StockPrice		price;				//ƽ���ɽ���
		TradeSide		side;				//��������
		StockCode		stock;				//֤ȯ����
		ExecuteType		lastExecType;		//����ִ�н��
		StockPrice		lastExecPrice;		//���γɽ�����
		StockNum		lastExecNum;		//���γɽ�����
		StockNum		leavesNum;			//δ�ɽ�����
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

	//��������ʵ��
	struct CancelOrder{
		OriOrderId		oriOrderId;			//������ί�����󵥺ţ�����ǰί�м�¼�л�ȡ
		StockNum		num;				//��������
		TradeSide		side;				//��������
		StockCode		stock;				//֤ȯ����
		OrderId			orderId;			//����ί�е���
		TradeRespBase	response;			//���������Ӧ�ö˷���������볷������޹�
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

	//�����ر�ʵ��
	struct CancelResponse{
		OrderId			orderId;			//����ί�е���
		OriOrderId		oriOrderId;			//������ί�����󵥺�
		CancelStatus	status;				//����ί��״̬
		RetMessage		rejectReason;		//�ܾ�ԭ��
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

	//�ֲֲ�ѯʵ��
	struct OrderQuery{
		OrderId			orderId;			//�ֲֲ�ѯ���󵥺ţ���Ӧ�޼��µ��ĵ��ţ������������
		TradeSide		side;				//��������
		StockCode		stock;				//֤ȯ����
		TradeRespBase	response;			//��ѯ���������Ӧ�ö˷�����������������޹�
		OrderQuery(
			OrderId orderId,
			TradeSide side,
			StockCode stock)
			:side(side),
			stock(stock),
			orderId(orderId){}
		OrderQuery(){}
	};

	//�ֲֲ�ѯ���
	struct OrderQueryResponse{
		RespOrderId		orderId;			//�ֲֲ�ѯӦ���ʶ����ֲֲ�ѯ���󵥺š��޼��µ����Ŷ�Ӧ
		StockNum		totalSum;			//�ۼƳɽ�����
		StockPrice		averageExecPrice;	//ƽ���ɽ��۸�
		StockNum		orderNum;			//ί��������
		OrderStatus		status;				//����״̬
		StockPrice		orderPrice;			//ί�м۸�
		TradeSide		side;				//��������
		StockCode		stock;				//֤ȯ����
		StockNum		leavesNum;			//δ�ɽ�����
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

	//�ʽ�ɷݲ�ѯʵ��
	struct FundPosQuery{
		FundPosReqId		id;				//�ʽ�ɷݲ�ѯ�����
		FundPosReqType		type;			//��ѯ���࣬�ɷ�0���ʽ�9
		TradeRespBase	response;			//��ѯ���������Ӧ�ö˷�����������������޹�
		FundPosQuery(
			FundPosReqType type)
			:type(type){}
		FundPosQuery(){}
	};

	struct NoPosition{
		PositionType	type;				//�ɷ����ͣ��ɷ����/�ɷݿ������/��ǰӵ�����ȵ�
		PositionNum		longQty;			//�������
		PositionNum		shortQty;			//�ղ�����
		NoPosition(
			PositionType type, 
			PositionNum longQty,
			PositionNum shortQty)
			:type(type),
			longQty(longQty),
			shortQty(shortQty){}
	};

	struct NoPosAmt{
		PosAmtType		type;				//������ͣ��ʽ����/�ʽ�������/�ʲ��ܶ��
		PosAmount		amount;				//���
		NoPosAmt(
			PosAmtType type,
			PosAmount amount)
			:type(type),
			amount(amount){}
	};

	typedef std::list<NoPosition> Positions;

	typedef std::list<NoPosAmt> Amounts;

	//�ʽ�ɷݲ�ѯ���
	struct FundPosQueryResponse{
		FundPosRespId			id;				//�ʽ�ɷݲ�ѯ����ţ����ѯ�����Ӧ
		FundPosReqType			type;			//�������
		RespCode				response;		//����״̬���ɹ�/ʧ��
		RetMessage				description;	//ʧ��˵��
		Currency				currency;		//����
		StockCode				stock;			//֤ȯ����
		MarketCode				market;			//�г�����
		ShareHolderCode			account;		//�ɶ����룬���������Ϊ�ɷ�ʱ
		Positions				positions;		//�ɷ�����
		Amounts					amounts;		//�ʽ�����
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