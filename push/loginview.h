
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "view.h"



namespace cure {
class LoginId;
class RuntimeVariableScope;
}



namespace Push {



class ClientLoginObserver;



class LoginView: public View {
public:
	LoginView(ClientLoginObserver* login_observer, const wstr& error_message);

private:
	void OnExit();
	void OnLogin(uitbc::Button*);

	ClientLoginObserver* login_observer_;
};



class ClientLoginObserver {
public:
	virtual void CancelLogin() = 0;
	virtual void RequestLogin(const str& server_address, const cure::LoginId& login_token) = 0;
	virtual cure::RuntimeVariableScope* GetVariableScope() const = 0;
};



}
