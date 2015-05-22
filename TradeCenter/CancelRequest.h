#ifndef CANCEL_REQUEST_H
#define CANCEL_REQUEST_H
#include "quickfix/fix42/OrderCancelRequest.h"
#include "TradeEntity.h"
#include "TradeError.h"

namespace NQ{
	class CancelRequest : public FIX42::OrderCancelRequest
	{
	public:
		CancelRequest(CancelOrder& cancel) : 
			FIX42::OrderCancelRequest(
			FIX::OrigClOrdID(cancel.oriOrderId),
			FIX::ClOrdID(GenerateUniqueID()),
			FIX::Symbol(cancel.stock),
			FIX::Side(cancel.side),
			FIX::TransactTime()){
				cancel.orderId = this->getField(FIX::FIELD::ClOrdID);
				cancel.response = TradeRespBase(RespCode::SUCC,"");
				set(FIX::OrderQty(cancel.num));
		}

		// 生成撤单回报
		static NQ::CancelResponse genCancelResponse(const FIX::Message &message) 
			//throw(NQ::MessageIncorrectError, NQ::WrongResponseError )
		{
			// 消息类型
			FIX::MsgType msgType;
			if (!message.getHeader().getFieldIfSet(msgType))
			{
				throw NQ::LackOfFieldError("MsgType 35");
			}

			// 撤单请求单号
			FIX::ClOrdID clOrdID;
			if (!message.getFieldIfSet(clOrdID))
			{
				throw NQ::LackOfFieldError("ClOrdID 11");
			}

			// 报单状态
			FIX::OrdStatus status;
			if (!message.getFieldIfSet(status))
			{
				throw NQ::LackOfFieldError("OrdStatus 39");
			}

			// 回报单号
			FIX::OrderID orderId;
			if (!message.getFieldIfSet(orderId))
			{
				throw NQ::LackOfFieldError("OrderID 37");
			}

			FIX::OrigClOrdID oriClOrdId;
			if (!message.getFieldIfSet(oriClOrdId))
			{
				throw NQ::LackOfFieldError("OrigClOrdID 41");
			}

			// 撤单被拒绝
			if (msgType == FIX::MsgType_OrderCancelReject)
			{
				FIX::Text rejReason;
				if (!message.getFieldIfSet(rejReason))
				{
					throw NQ::LackOfFieldError("Text 58");
				}

				CancelResponse cancelResp(
					clOrdID.getValue(),
					oriClOrdId.getValue(),
					CancelStatus::CancelRejected,
					rejReason.getValue());
				return cancelResp;
			}

			// 撤单被接受
			if (msgType == FIX::MsgType_ExecutionReport)
			{
				// 等待撤单
				if (status == FIX::OrdStatus_PENDING_CANCEL)
				{
					CancelResponse cancelResp(
						clOrdID.getValue(),
						oriClOrdId.getValue(),
						CancelStatus::CancelConfirmed);
					return cancelResp;
				}
				// 撤单执行
				if (status == FIX::OrdStatus_CANCELED)
				{
					CancelResponse cancelResp(
						clOrdID.getValue(),
						oriClOrdId.getValue(),
						CancelStatus::CancelExecuted);
					return cancelResp;
				}
			}

			throw NQ::WrongResponseError("Not for cancel order response");
		}

	};
}


#endif // !CANCEL_REQUEST_H
