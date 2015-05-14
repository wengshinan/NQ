#ifndef TRADE_ERROR_H
#define TRADE_ERROR_H

#include "quickfix/Exceptions.h"

namespace NQ{

	struct RunningError : public FIX::Exception{
		RunningError( const std::string& t, const std::string& d ):
			FIX::Exception(t,d){}
	};

	// δ��¼ʱ��������
	struct ReqWhenNotLogonError : public NQ::RunningError
	{
		ReqWhenNotLogonError( const std::string& what = "" )
			: RunningError( "Send request when not logon error", what ) {}
	};

	// ���Ĳ���ȷ
	struct MessageIncorrectError : public NQ::RunningError
	{
		MessageIncorrectError( const std::string& what = "")
			: RunningError( "Message received not correct error", what ) {}
	};

	// �������
	struct WrongRequestError : public NQ::RunningError
	{
	};

	// �ر�����
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
