
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "View.h"



namespace Cure
{
class LoginId;
class RuntimeVariableScope;
}



namespace Life
{



class ClientLoginObserver;



class ClientLoginView: public View
{
public:
	ClientLoginView(ClientLoginObserver* pLoginObserver, const Lepra::String& pErrorMessage);

private:
	void OnExit();
	void OnLogin(UiTbc::Button*);

	ClientLoginObserver* mLoginObserver;
};



class ClientLoginObserver
{
public:
	virtual void CancelLogin() = 0;
	virtual void RequestLogin(const Lepra::String& pServerAddress, const Cure::LoginId& pLoginToken) = 0;
	virtual Cure::RuntimeVariableScope* GetVariableScope() const = 0;
};



}
