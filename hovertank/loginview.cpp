
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



namespace HoverTank {



LoginView::LoginView(ClientLoginObserver* login_observer, const str& error_message):
	View("Login", new uitbc::GridLayout(7, 1)),
	login_observer_(login_observer) {
	SetPreferredSize(200, 200);

	if (!error_message.empty()) {
		((uitbc::GridLayout*)GetClientRectComponent()->GetLayout())->InsertRow(0);
		AddLabel(error_message, RED);
	}

	AddLabel("Username", WHITE);

	str user_name;
	v_tryget(user_name, =, login_observer_->GetVariableScope(), kRtvarLoginUsername, "User0");
	AddTextField(user_name, "User");

	AddLabel("Password", WHITE);

	AddTextField("CarPassword", "Pass")->SetPasswordCharacter('*');

	AddLabel("Server", WHITE);

	str server_name;
	v_get(server_name, =, login_observer_->GetVariableScope(), kRtvarNetworkServeraddress, "localhost:16650");
	if (strutil::StartsWith(server_name, "0.0.0.0")) {
		server_name = server_name.substr(7);
	}
	AddTextField(server_name, "Server");

	AddButton("Login", 0)->SetOnClick(LoginView, OnLogin);

	UpdateLayout();
}



void LoginView::OnExit() {
	login_observer_->CancelLogin();
}

void LoginView::OnLogin(uitbc::Button*) {
	// Save user's login info.
	str server = ((uitbc::TextField*)GetChild("Server", 0))->GetText();

	// Pick strings from kUi.
	wstr username = wstrutil::Encode(((uitbc::TextField*)GetChild("User", 0))->GetText());
	uitbc::TextField* password_component = (uitbc::TextField*)GetChild("Pass", 0);
	wstr readable_password = wstrutil::Encode(password_component->GetText());
	password_component->SetText("?");	// Clear out password traces in component.

	// Convert into login format.
	cure::MangledPassword password(readable_password);
	readable_password.clear();	// Clear out password traces in string.
	cure::LoginId login_token(username, password);

	login_observer_->RequestLogin(server, login_token);
}



}
