#ifndef NEW_ORDER_SINGLE_H
#define NEW_ORDER_SINGLE_H
#include "quickfix/fix42/NewOrderSingle.h"
#include "TradeEntity.h"
#include "TradeError.h"

namespace NQ{
	class NewLimitedOrderSingle : public FIX42::NewOrderSingle
	{

	public:
		NewLimitedOrderSingle(
			LimitedOrder& order ) :  
		FIX42::NewOrderSingle(
			FIX::ClOrdID(GenerateUniqueID()),
			FIX::HandlInst(FIX::HandlInst_AUTOMATED_EXECUTION_ORDER_PRIVATE_NO_BROKER_INTERVENTION),
			FIX::Symbol(order.stock),
			FIX::Side(order.side),
			FIX::TransactTime(),
			FIX::OrdType(FIX::OrdType_LIMIT))
		{
			set(FIX::Price(order.price));
			set(FIX::Currency(EnumCurrency[order.currency]));
			set(FIX::SecurityExchange(EnumMarketCode[order.market]));
			set(FIX::OrderQty(order.num));
			if (order.openCloseFlag == OpenCloseFlag::FlagOpen
				|| order.openCloseFlag == OpenCloseFlag::FlagClose)
			{				
				set(FIX::OpenClose(order.openCloseFlag));
			}
			if (order.cashMargin == CashMargin::MarginOpen
				|| order.cashMargin == CashMargin::MarginClose
				|| order.cashMargin == CashMargin::MarginCash)
			{
				setField(FIX::CashMargin(order.cashMargin));
			}
			order.id = this->getField(FIX::FIELD::ClOrdID);
			order.response = TradeRespBase(RespCode::SUCC,"");
		}

		// ����ί��ִ�лر�
		static NQ::ExecuteResponse genExecuteResponse(const FIX::Message &message)
			//throw ( NQ::MessageIncorrectError, NQ::WrongResponseError )
		{
			OrderStatus ordStatus;
			ExecuteType execType;
			// ���󵥺�
			FIX::ClOrdID msgClOrdID;
			if (!message.getFieldIfSet(msgClOrdID))
			{
				throw NQ::LackOfFieldError("ClOrdID 11");
			}
			// ����״̬
			FIX::OrdStatus status;
			if (!message.getFieldIfSet(status))
			{
				throw NQ::LackOfFieldError("OrdStatus 39");
			}
			if (status != FIX::OrdStatus_FILLED 
				&& status != FIX::OrdStatus_PARTIALLY_FILLED
				&& status != FIX::OrdStatus_PENDING_CANCEL)
			{
				throw NQ::LackOfFieldError("Not for order executed response");
			}

			// ִ�н��
			FIX::ExecType execStatus;
			if (!message.getFieldIfSet(execStatus))
			{
				throw NQ::LackOfFieldError("ExecType 150");
			}
			if (execStatus != FIX::ExecType_FILL 
				&& execStatus != FIX::ExecType_PARTIAL_FILL)
			{
				throw NQ::WrongResponseError("Not for order executed response");
			}
			// �ܾ�ԭ��
			/*
			FIX::OrdRejReason rejCode;
			FIX::Text rejText;
			message.getFieldIfSet(rejCode);
			message.getFieldIfSet(rejText);
			*/

			// ί��ȫ���ɽ�
			if (status == FIX::OrdStatus_FILLED)
			{
				ordStatus = OrderStatus::OrderAllExecuted;
				//����ȫ���ɽ�
				if (execStatus == FIX::ExecType_FILL)	
				{
					execType = ExecuteType::AllExecuted;
				}
				// ���β��ֳɽ�
				else {
					execType = ExecuteType::PartlyExecuted;
				}

			}
			// ί�в��ֳɽ�
			else if (status == FIX::OrdStatus_PARTIALLY_FILLED)
			{
				ordStatus = OrderStatus::OrderPartlyExecuted;
				execType = ExecuteType::PartlyExecuted;
			}
			// ί�д���
			else
			{
				ordStatus = OrderStatus::OrderCancelling;
				execType = ExecuteType::PartlyExecuted;
			}
			// ί������
			FIX::CumQty orderNum;
			if (!message.getFieldIfSet(orderNum))
			{
				throw NQ::LackOfFieldError("CumQty 14");
			}
			// ί�м۸�
			FIX::Price orderPrice;
			message.getFieldIfSet(orderPrice);
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
			//���γɽ��۸�
			FIX::LastPx lastPx;
			message.getFieldIfSet(lastPx);
			//���γɽ�����
			FIX::LastShares lastShare;
			message.getFieldIfSet(lastShare);
			//δ�ɽ�����
			FIX::LeavesQty leavesNum;
			message.getFieldIfSet(leavesNum);

			// ���ر����ر���ֵ
			ExecuteResponse orderResp(
				msgClOrdID.getValue(),
				ordStatus,
				orderNum.getValue(),
				message.isSetField(orderPrice) ? orderPrice.getValue() : 0,
				EnumTradeSide[side.getValue()-'1'],
				stock.getValue(),
				execType,
				lastPx ? lastPx.getValue() : 0,
				lastShare ? lastShare.getValue() : 0,
				leavesNum ? leavesNum.getValue() : 0
				);
			return orderResp;
		}

		// ���ɱ����ر�
		static NQ::OrderResponse genOrderResponse(const FIX::Message &message)
		{
			// ���󵥺�
			FIX::ClOrdID msgClOrdID;
			if (!message.getFieldIfSet(msgClOrdID))
			{
				throw NQ::LackOfFieldError("ClOrdID 11");
			}
			// ����״̬
			FIX::OrdStatus status;
			if (!message.getFieldIfSet(status))
			{
				throw NQ::LackOfFieldError("OrdStatus 39");
			}
			if (status != FIX::OrdStatus_NEW
				&& status != FIX::OrdStatus_REJECTED)
			{
				throw NQ::WrongResponseError("Not for order response");
			}
			NQ::OrderStatus ordStatus;
			// ί��ȷ��
			if (status == FIX::OrdStatus_NEW){
				ordStatus = OrderStatus::OrderConfirmed;
				OrderResponse orderResp(msgClOrdID,ordStatus);
				return orderResp;
			}
			// ί�оܾ�
			else{
				// �ܾ�ԭ��
				FIX::Text rejText;
				message.getFieldIfSet(rejText);
				ordStatus = OrderStatus::OrderRejected;
				OrderResponse orderResp(msgClOrdID,ordStatus,rejText);
				return orderResp;
			}
		}
	};

}


#endif // !NEW_ORDER_SINGLE_H
