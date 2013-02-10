
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "View.h"



namespace Cure
{
class LoginId;
class RuntimeVariableScope;
}



namespace Push
{



class ClientLoginObserver;



class LoginView: public View
{
public:
	LoginView(ClientLoginObserver* pLoginObserver, const str& pErrorMessage);

private:
	void OnExit();
	void OnLogin(UiTbc::Button*);

	ClientLoginObserver* mLoginObserver;
};



class ClientLoginObserver
{
public:
	virtual void CancelLogin() = 0;
	virtual void RequestLogin(const str& pServerAddress, const Cure::LoginId& pLoginToken) = 0;
	virtual Cure::RuntimeVariableScope* GetVariableScope() const = 0;
};



}