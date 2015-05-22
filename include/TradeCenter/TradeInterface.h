#ifndef TRADE_INTERFACE_H
#define TRADE_INTERFACE_H

#include "TradeEntity.h"

namespace NQ{
	__interface IOrderCaller
	{
	public:
		//�޼��µ�����Ӧ����TradeCenter::reqLimitedOrder()����������
		virtual int ReqLimitOrder(AccountNo user, LimitedOrder& order)=0;
		//��������
		virtual int ReqCancelLimitOrder(AccountNo user, CancelOrder& order)=0;
		//�ֲֲ�ѯ
		virtual int ReqOrderQuery(AccountNo user, OrderQuery& query)=0;
		//�ʽ�ɷݲ�ѯ
		virtual int ReqFundPosQuery(AccountNo user, FundPosQuery& query)=0;
		

		//�޼��µ��ص�
		virtual int OnRespLimitOrder(AccountNo user, OrderResponse response)=0;
		//�޼��µ��ɽ��ص�
		virtual int OnRespExecuteLimitOrder(AccountNo user, ExecuteResponse response)=0;
		//�����ر��ص�
		virtual int OnRespCancelLimitOrder(AccountNo user, CancelResponse response)=0;
		//�ֲֲ�ѯ�ر��ص�
		virtual int OnRespOrderQuery(AccountNo user, OrderQueryResponse response)=0;
		//�ʽ�ɷݲ�ѯ�ر��ص�
		virtual int OnRespFundPosQuery(AccountNo user, FundPosQueryResponse response)=0;
	};

	__interface IUser
	{
	public:
		virtual int LoadUser()=0; //ͨ�������ļ�����ʼ�����棬TradeCenter::init(); start()
		virtual int UnloadUser(AccountNo user)=0;  //TradeCenter::stop()
	};

}


#endif // !TRADE_INTERFACE_H
