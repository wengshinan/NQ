#ifndef FUND_POSITIONS_QUERY_REQUEST_H
#define FUND_POSITIONS_QUERY_REQUEST_H

#include "quickfix/Message.h"
#include "quickfix/Group.h"
#include "TradeEntity.h"
#include "TradeError.h"

namespace NQ
{
	class FundPositionsQueryRequest : public FIX42::Message
	{
	public:
		class NoPositions: public FIX::Group
		{
		public:
			NoPositions() : FIX::Group(702,703,FIX::message_order(703,704,0)) {}
			FIELD_SET(*this, FIX::PosType);
			FIELD_SET(*this, FIX::LongQty);
			FIELD_SET(*this, FIX::ShortQty);
		};
		class NoPosAmts: public FIX::Group
		{
		public:
			NoPosAmts() : FIX::Group(753,707,FIX::message_order(707,708,0)) {}
			FIELD_SET(*this, FIX::PosAmtType);
			FIELD_SET(*this, FIX::PosAmt);
		};
	public:
		FundPositionsQueryRequest() : Message(MsgType()) {}
		FundPositionsQueryRequest(const FIX::Message& m) : Message(m) {}
		FundPositionsQueryRequest(const Message& m) : Message(m) {}
		FundPositionsQueryRequest(const FundPositionsQueryRequest& m) : Message(m) {}
		static FIX::MsgType MsgType() { return FIX::MsgType(MsgType_UAN); }

		/*
		FundPositionsQueryRequest(
		const FIX::PosReqID& aPosReqID,
		const FIX::PosReqType& aPosReqType )
		: Message(MsgType())
		{
		set(aPosReqID);
		set(aPosReqType);
		}
		*/
		FundPositionsQueryRequest(FundPosQuery& query): Message(MsgType()){
			query.id = GenerateUniqueID();
			//FundPositionsQueryRequest(query.id,query.type);
			set(query.id);
			set(query.type);
			query.response = TradeRespBase(RespCode::SUCC,"");
		}

		FIELD_SET(*this, FIX::PosReqID);
		FIELD_SET(*this, FIX::PosReqType);


	public:
		// �����ʽ�ɷݲ�ѯ�ر������Σ�
		static NQ::FundPosQueryResponse genFundPosQryResponse(const FIX::Message &message) 
		{
			// ��Ϣ����
			FIX::MsgType msgType;
			if (!message.getHeader().getFieldIfSet(msgType))
			{
				throw NQ::LackOfFieldError("MsgType 35");
			}

			if (msgType != MsgType_UAP)
			{
				throw NQ::WrongResponseError("Not for fund/positions query response");
			}

			// ��Ӧ��Ϣid
			FIX::PosReqID rptId;
			if (!message.getFieldIfSet(rptId))
			{
				throw NQ::LackOfFieldError("PosReqID 710");
			}

			// �������
			FIX::PosReqType posReqType;
			if (!message.getFieldIfSet(posReqType))
			{
				throw NQ::LackOfFieldError("PosReqType 724");
			}

			if (posReqType != FIX::PosReqType_POSITIONS
				&& posReqType != NQ::PosReqType_FUNDS)
			{
				throw NQ::WrongResponseError("Wrong value in [PosReqType 724] for fund/positions query response");
			}

			// ���淵��״̬
			FIX::PosReqResult posReqStatus;
			if (!message.getFieldIfSet(posReqStatus))
			{
				throw NQ::LackOfFieldError("PosReqResult 728");
			}

			// ����ʧ��
			if (posReqStatus != FIX::PosReqResult_VALID_REQUEST)
			{
				FIX::Text description;
				if (!message.getFieldIfSet(description))
				{
					throw NQ::LackOfFieldError("Text 58");
				}
				FundPosQueryResponse response(rptId.getValue(),posReqType.getValue(),RespCode::FAIL,description);
				return response;
			}

			Positions positions;
			Amounts amounts;
			//EnumParser<PositionType>* posParser = new EnumParser<PositionType>();

			if (posReqType == FIX::PosReqType_POSITIONS)
			{
				// �ɷ�group����
				FIX::NoPositions posNum;
				if (!message.getFieldIfSet(posNum))
				{
					throw NQ::LackOfFieldError("NoPositions 702");
				}
				NQ::FundPositionsQueryRequest::NoPositions repeatingPosItem;
				EnumParser<PositionType> posParser;
				if (posNum.getValue() != 0)
				{
					for (int i = 1; i <= posNum.getValue(); i++)
					{
						message.getGroup( i, repeatingPosItem );
						NoPosition position(
							posParser.parseEnum(repeatingPosItem.getField(FIX::FIELD::PosType)),
							repeatingPosItem.isSetField(FIX::FIELD::LongQty)?
							atoi(repeatingPosItem.getField(FIX::FIELD::LongQty).c_str()):0,
							repeatingPosItem.isSetField(FIX::FIELD::ShortQty)?
							atoi(repeatingPosItem.getField(FIX::FIELD::ShortQty).c_str()):0);
						positions.push_back(position);
					}
				}
				//delete posParser;
			}
			else{
				FIX::NoPosAmt posAmtNum;
				if (!message.getFieldIfSet(posAmtNum))
				{
					throw NQ::LackOfFieldError("NoPosAmt 753");
				}
				NQ::FundPositionsQueryRequest::NoPosAmts repeatingPosAmtItem;
				//EnumParser<PosAmtType>* posAmtParser = new EnumParser<PosAmtType>();
				EnumParser<PosAmtType> posAmtParser;
				if (posAmtNum.getValue()!= 0)
				{
					for (int i = 1; i <= posAmtNum.getValue(); i++)
					{
						message.getGroup( i, repeatingPosAmtItem );
						NoPosAmt amt(
							posAmtParser.parseEnum(repeatingPosAmtItem.getField(FIX::FIELD::PosAmtType)),
							repeatingPosAmtItem.isSetField(FIX::FIELD::PosAmt)?
							atoi(repeatingPosAmtItem.getField(FIX::FIELD::PosAmt).c_str()):0);
						amounts.push_back(amt);
					}
				}
			}
			//delete posAmtParser;
			// �����ѯ�ر�����ֵ
			NQ::FundPosQueryResponse response(rptId.getValue(),posReqType.getValue(),RespCode::SUCC,"");
			response.positions = positions;
			response.amounts = amounts;
			FIX::Currency currency;
			if (message.getFieldIfSet(currency))
			{
				response.currency = (new EnumParser<Currency>())->parseEnum(currency);
			}
			FIX::Symbol stock;
			if (message.getFieldIfSet(stock))
			{
				response.stock = stock.getValue();
			}
			FIX::SecurityExchange market;
			if (message.getFieldIfSet(market))
			{
				response.market = (new EnumParser<MarketCode>())->parseEnum(market);
			}
			FIX::Account account;
			if (message.getFieldIfSet(account))
			{
				response.account = account.getValue();
			}
			return response;

		}

		//�Ƿ�Ψһһ��
		static bool isSingleReport(const FIX::Message &message)
		{
			// ���ر�������
			FIX::TotalNumPosReports totalNum;
			if (!message.getFieldIfSet(totalNum))
			{
				throw NQ::LackOfFieldError("TotalNumPosReports 727");
			}
			return totalNum.getValue() == 1;
		}

		//�Ƿ�൥�����һ��
		static bool isLastReport(const FIX::Message &message)
		{
			FIX::LastRptRequested lastRpt;
			if (!message.getFieldIfSet(lastRpt))
			{
				throw NQ::LackOfFieldError("LastRptRequested 912");
			}
			return lastRpt.getValue();
		}

		//�ϲ�����UAP�ص�
		static void mergeRespnse(FundPosQueryResponse &resp, std::list<FundPosQueryResponse> &resps)
		{
			for (std::list<FundPosQueryResponse>::iterator it = resps.begin(); it != resps.end(); ++it)
			{
				if ((*it).id == resp.id)
				{
					for (std::list<NoPosition>::iterator itn = (*it).positions.begin(); 
						itn != (*it).positions.end(); ++itn)
					{
						NoPosition tmpPosition(*itn);
						resp.positions.push_back(tmpPosition);
					}
					for (std::list<NoPosAmt>::iterator itn = (*it).amounts.begin(); 
						itn != (*it).amounts.end(); ++itn)
					{
						NoPosAmt tmpAmt(*itn);
						resp.amounts.push_back(tmpAmt);
					}
				}
				FundPosQueryResponse itp = *it;
				delete &itp;
				resps.erase(it);
			}
		}
	};


}


#endif // !FUND_POSITIONS_QUERY_REQUEST_H
