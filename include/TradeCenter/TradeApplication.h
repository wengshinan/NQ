#pragma once
#ifndef TRADE_APPLICATION_H
#define TRADE_APPLICATION_H

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "TradeBase.h"
#include "TradeEntity.h"
#include "NewOrderSingle.h"
#include "CancelRequest.h"
#include "OrderQueryRequest.h"
#include "FundPositionsQueryRequest.h"
#include "Lock.h"
#include "TradeError.h"
#include <list>
#include <map>
#include <thread>
#include <iostream>
#include <utility>

#define Key std::string
#define FilePath std::string
#define itoa _itoa_s

namespace NQ{
	class TradeCenter;

	class TradeApplication : 
		public FIX::Application
	{
	public:
		TradeApplication(void);
		TradeApplication(
			const FilePath& configFilePath);
		~TradeApplication(void);
		typedef std::map<std::string, FIX::Message*> Orders;
		typedef std::list<FIX::Message*> Reqs;
		typedef std::list<OrderResponse> OrdResps;
		typedef std::list<ExecuteResponse> ExecResps;
		typedef std::list<CancelResponse> CancResps;
		typedef std::list<OrderQueryResponse> OrdQryResps;
		typedef std::list<FundPosQueryResponse> FundPosQryResps;

	public:
		void setInitiator(FIX::SocketInitiator* initiator){this->m_initiator=initiator;};
		void sendFundPosQry(FundPosQuery& query);
		void sendLimitedOrder(LimitedOrder& order);
		void sendCancelOrder(CancelOrder& cancel);
		void sendQueryOrder(OrderQuery& query);
		void runSendMessage();
		bool isConnected();
		bool isConnecting();
		bool isSendThreadRunning();
		bool isRecvThreadRunning();
		UserConfiguration* getUserConfig();
		void setCaller(TradeCenter* tradeCenter);

	public:
		static TradeRespBase setResponse(RespCode respCode, TradeResult errorMessage);

	private:
		void sendThread();
		void receveThread();
		void onCreate( const FIX::SessionID &);
		void onLogon( const FIX::SessionID &sessionID );
		void onLogout( const FIX::SessionID &sessionID );
		void toAdmin( FIX::Message &msg, const FIX::SessionID &);
		void toApp( FIX::Message&, const FIX::SessionID &) throw( FIX::DoNotSend );
		void fromAdmin( const FIX::Message&, const FIX::SessionID &)
			throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon );
		void fromApp( const FIX::Message &message, const FIX::SessionID &sessionID )
			throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );
		void initSessionLog(const FIX::SessionID &sessionID );

	private:
		void setConfig();
		void setLogonConfig(FIX::Message &message);
		ReqType getReqType(const FIX::Message &message);
		RespType getRespType(const FIX::Message &message);
		bool isRespCorresponant(const FIX::Message &message);

	private:
		EncryptMethod						g_strEncryptType;
		Key									g_strPublicKey;
		UserConfiguration					g_user;
		ConnectStatus						g_sconnected;
		FilePath							g_pszAppConfigFile;
		FIX::SessionID						g_sessionId;

		Reqs								g_reqList;
		Orders								g_orders;
		OrdResps							g_ordResps;
		ExecResps							g_execResps;
		CancResps							g_cancResps;
		OrdQryResps							g_ordQryResps;
		FundPosQryResps						g_fundPosQryResps;


		CRITICAL_SECTION					g_cs_s;
		CRITICAL_SECTION					g_cs_r;
		std::thread*						g_sThread;
		std::thread*						g_rThread;
		FIX::FileLogFactory*				g_logFactory;
		FIX::Log*							g_log;
		std::string							g_preLogStr;
		FIX::SocketInitiator*				m_initiator;
		NQ::TradeCenter*					c_tradeCenter;
	};
}

#endif // !TRADE_APPLICATION_H