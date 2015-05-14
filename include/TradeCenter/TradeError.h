#ifndef TRADE_ERROR_H
#define TRADE_ERROR_H

#include "quickfix/Exceptions.h"

namespace NQ{

	struct RunningError : public FIX::Exception{
		RunningError( const std::string& t, const std::string& d ):
			FIX::Exception(t,d){}
	};

	// 未登录时发送请求
	struct ReqWhenNotLogonError : public NQ::RunningError
	{
		ReqWhenNotLogonError( const std::string& what = "" )
			: RunningError( "Send request when not logon error", what ) {}
	};

	// 报文不正确
	struct MessageIncorrectError : public NQ::RunningError
	{
		MessageIncorrectError( const std::string& what = "")
			: RunningError( "Message received not correct error", what ) {}
	};

	// 请求错误
	struct WrongRequestError : public NQ::RunningError
	{
	};

	// 回报错误
	struct WrongResponseError : public NQ::RunningError
	{
		WrongResponseError( const std::string& what = "")
			: RunningError( "Response not correct error", what ) {}
	};

	struct LackOfFieldError : public NQ::MessageIncorrectError
	{
		LackOfFieldError( const std::string& what = "")
			: MessageIncorrectError( "Lack of field [" + what + "]" ){}
	};

	struct NotCorrespondingError : public NQ::WrongResponseError
	{
		NotCorrespondingError()
			: WrongResponseError("Response not corresponding request"){}
	};

}
#endif // !TRADE_ERROR_H
