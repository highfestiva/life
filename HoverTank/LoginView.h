
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "View.h"



namespace Cure
{
class LoginId;
class RuntimeVariableScope;
}



namespace HoverTank
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
