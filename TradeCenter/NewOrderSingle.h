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

		// 生成委托执行回报
		static NQ::ExecuteResponse genExecuteResponse(const FIX::Message &message)
			//throw ( NQ::MessageIncorrectError, NQ::WrongResponseError )
		{
			OrderStatus ordStatus;
			ExecuteType execType;
			// 请求单号
			FIX::ClOrdID msgClOrdID;
			if (!message.getFieldIfSet(msgClOrdID))
			{
				throw NQ::LackOfFieldError("ClOrdID 11");
			}
			// 报单状态
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

			// 执行结果
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
			// 拒绝原因
			/*
			FIX::OrdRejReason rejCode;
			FIX::Text rejText;
			message.getFieldIfSet(rejCode);
			message.getFieldIfSet(rejText);
			*/

			// 委托全部成交
			if (status == FIX::OrdStatus_FILLED)
			{
				ordStatus = OrderStatus::OrderAllExecuted;
				//本次全部成交
				if (execStatus == FIX::ExecType_FILL)	
				{
					execType = ExecuteType::AllExecuted;
				}
				// 本次部分成交
				else {
					execType = ExecuteType::PartlyExecuted;
				}

			}
			// 委托部分成交
			else if (status == FIX::OrdStatus_PARTIALLY_FILLED)
			{
				ordStatus = OrderStatus::OrderPartlyExecuted;
				execType = ExecuteType::PartlyExecuted;
			}
			// 委托待撤
			else
			{
				ordStatus = OrderStatus::OrderCancelling;
				execType = ExecuteType::PartlyExecuted;
			}
			// 委托数量
			FIX::CumQty orderNum;
			if (!message.getFieldIfSet(orderNum))
			{
				throw NQ::LackOfFieldError("CumQty 14");
			}
			// 委托价格
			FIX::Price orderPrice;
			message.getFieldIfSet(orderPrice);
			// 买卖方向
			FIX::Side side;
			if (!message.getFieldIfSet(side))
			{
				throw NQ::LackOfFieldError("Side 54");
			}
			// 证券代码
			FIX::Symbol stock;
			if (!message.getFieldIfSet(stock))
			{
				throw NQ::LackOfFieldError("Symbol 55");
			}
			//本次成交价格
			FIX::LastPx lastPx;
			message.getFieldIfSet(lastPx);
			//本次成交股数
			FIX::LastShares lastShare;
			message.getFieldIfSet(lastShare);
			//未成交数量
			FIX::LeavesQty leavesNum;
			message.getFieldIfSet(leavesNum);

			// 返回报单回报赋值
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

		// 生成报单回报
		static NQ::OrderResponse genOrderResponse(const FIX::Message &message)
		{
			// 请求单号
			FIX::ClOrdID msgClOrdID;
			if (!message.getFieldIfSet(msgClOrdID))
			{
				throw NQ::LackOfFieldError("ClOrdID 11");
			}
			// 报单状态
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
			// 委托确认
			if (status == FIX::OrdStatus_NEW){
				ordStatus = OrderStatus::OrderConfirmed;
				OrderResponse orderResp(msgClOrdID,ordStatus);
				return orderResp;
			}
			// 委托拒绝
			else{
				// 拒绝原因
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
