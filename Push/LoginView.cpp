
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "loginview.h"
#include "../cure/include/runtimevariable.h"
#include "../cure/include/useraccount.h"
#include "../lepra/include/network.h"
#include "../uitbc/include/gui/uicaption.h"
#include "../uitbc/include/gui/uitextfield.h"
#include "rtvar.h"



namespace Push {



LoginView::LoginView(ClientLoginObserver* login_observer, const wstr& error_message):
	View(L"Login", new uitbc::GridLayout(7, 1)),
	login_observer_(login_observer) {
	SetPreferredSize(200, 200);

	if (!error_message.empty()) {
		((uitbc::GridLayout*)GetClientRectComponent()->GetLayout())->InsertRow(0);
		AddLabel(error_message, RED);
	}

	AddLabel(L"Username", WHITE);

	str user_name;
	v_tryget(user_name, =, login_observer_->GetVariableScope(), kRtvarLoginUsername, "User0");
	AddTextField(wstrutil::Encode(user_name), "User");

	AddLabel(L"Password", WHITE);

	AddTextField(L"CarPassword", "Pass")->SetPasswordCharacter('*');

	AddLabel(L"Server", WHITE);

	str server_name;
	v_get(server_name, =, login_observer_->GetVariableScope(), kRtvarNetworkServeraddress, "localhost:16650");
	if (strutil::StartsWith(server_name, "0.0.0.0")) {
		server_name = server_name.substr(7);
	}
	AddTextField(wstrutil::Encode(server_name), "Server");

	AddButton(L"Login", 0)->SetOnClick(LoginView, OnLogin);

	UpdateLayout();
}



void LoginView::OnExit() {
	login_observer_->CancelLogin();
}

void LoginView::OnLogin(uitbc::Button*) {
	// Save user's login info.
	str server = strutil::Encode(((uitbc::TextField*)GetChild("Server"))->GetText());

	// Pick strings from kUi.
	str username = strutil::Encode(((uitbc::TextField*)GetChild("User"))->GetText());
	uitbc::TextField* password_component = (uitbc::TextField*)GetChild("Pass");
	str readable_password = strutil::Encode(password_component->GetText());
	password_component->SetText(L"?");	// Clear out password traces in component.

	// Convert into login format.
	cure::MangledPassword password(readable_password);
	readable_password.clear();	// Clear out password traces in string.
	cure::LoginId login_token(username, password);

	login_observer_->RequestLogin(server, login_token);
}



}
