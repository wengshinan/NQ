#ifndef ORDER_QUERY_REQUEST_H
#define ORDER_QUERY_REQUEST_H
#include "quickfix/fix42//OrderStatusRequest.h"
#include "TradeEntity.h"
#include "TradeError.h"

namespace NQ{
	class OrderQueryRequest : public FIX42::OrderStatusRequest
	{
	public:
		OrderQueryRequest(OrderQuery& ordQuery):
			FIX42::OrderStatusRequest(
			FIX::ClOrdID(GenerateUniqueID()),
			FIX::Symbol(ordQuery.stock),
			FIX::Side(ordQuery.side)){
				ordQuery.orderId = this->getField(FIX::FIELD::ClOrdID);
				ordQuery.response = TradeRespBase(RespCode::SUCC,"");
		}

		// ���ɳֲ�״̬��ѯ�ر�
		static NQ::OrderQueryResponse genOrderQueryResponse(const FIX::Message &message) 
		{
			// ��Ϣ����
			FIX::MsgType msgType;
			if (!message.getFieldIfSet(msgType))
			{
				throw NQ::LackOfFieldError("MsgType 35");
			}

			if (msgType != FIX::MsgType_ExecutionReport)
			{
				throw NQ::WrongResponseError("Not for order query response");
			}

			// Ӧ�𵥺�
			FIX::OrderID ordID;
			if (!message.getFieldIfSet(ordID))
			{
				throw NQ::LackOfFieldError("OrderID 37");
			}

			// ƽ���ɽ���
			FIX::AvgPx avgPx;
			if (!message.getFieldIfSet(avgPx))
			{
				throw NQ::LackOfFieldError("AvgPx 6");
			}

			// �ۼƳɽ�����
			FIX::CumQty cumQty;
			if (!message.getFieldIfSet(cumQty))
			{
				throw NQ::LackOfFieldError("CumQty 14");
			}

			// ί������
			FIX::OrderQty orderQty;
			if (!message.getFieldIfSet(orderQty))
			{
				throw NQ::LackOfFieldError("OrderQty 38");
			}

			// ί��״̬
			FIX::OrdStatus ordStatus;
			if (!message.getFieldIfSet(ordStatus))
			{
				throw NQ::LackOfFieldError("OrdStatus 39");
			}

			NQ::OrderStatus status;
			if (ordStatus.getValue() == FIX::OrdStatus_FILLED)
			{
				status = OrderStatus::OrderAllExecuted;
			}
			else if (ordStatus == FIX::OrdStatus_PARTIALLY_FILLED)
			{
				status = OrderStatus::OrderPartlyExecuted;
			}
			else if (ordStatus == FIX::OrdStatus_PENDING_CANCEL)
			{
				status = OrderStatus::OrderCancelling;
			}
			else if (ordStatus == FIX::OrdStatus_CANCELED)
			{
				status = OrderStatus::OrderCacelled;
			}
			else if (ordStatus == FIX::OrdStatus_REJECTED)
			{
				status = OrderStatus::OrderRejected;
			}
			else if (ordStatus == FIX::OrdStatus_NEW)
			{
				status = OrderStatus::OrderConfirmed;
			}
			else
			{
				throw NQ::WrongResponseError("Not for order query response");
			}

			// ί�м۸�
			FIX::Price ordPrice;
			if (!message.getFieldIfSet(ordPrice))
			{
				throw NQ::LackOfFieldError("Price 44");
			}

			// ��������
			FIX::Side side;
			if (!message.getFieldIfSet(side))
			{
				throw NQ::LackOfFieldError("Side 54");
			}

			// ֤ȯ����
			FIX::Symbol stock;
			if (!message.getFieldIfSet(stock))
			{
				throw NQ::LackOfFieldError("Symbol 55");
			}

			// δ�ɽ�����
			FIX::LeavesQty leavesNum;
			if (!message.getFieldIfSet(leavesNum))
			{
				throw NQ::LackOfFieldError("LeavesQty 151");
			}

			OrderQueryResponse ordQryResult(
				ordID.getValue(),
				cumQty.getValue(),
				avgPx.getValue(),
				orderQty.getValue(),
				status,
				ordPrice.getValue(),
				TradeSide(side.getValue()),
				stock.getValue(),
				leavesNum.getValue());
			return ordQryResult;
		}
	};
}


#endif // !ORDER_QUERY_REQUEST_H
