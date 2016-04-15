
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "LoginView.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Cure/Include/UserAccount.h"
#include "../Lepra/Include/Network.h"
#include "../UiTbc/Include/GUI/UiCaption.h"
#include "../UiTbc/Include/GUI/UiTextField.h"
#include "RtVar.h"



namespace HoverTank
{



LoginView::LoginView(ClientLoginObserver* pLoginObserver, const str& pErrorMessage):
	View("Login", new UiTbc::GridLayout(7, 1)),
	mLoginObserver(pLoginObserver)
{
	SetPreferredSize(200, 200);

	if (!pErrorMessage.empty())
	{
		((UiTbc::GridLayout*)GetClientRectComponent()->GetLayout())->InsertRow(0);
		AddLabel(pErrorMessage, RED);
	}

	AddLabel("Username", WHITE);

	str lUserName;
	v_tryget(lUserName, =, mLoginObserver->GetVariableScope(), RTVAR_LOGIN_USERNAME, "User0");
	AddTextField(lUserName, "User");

	AddLabel("Password", WHITE);

	AddTextField("CarPassword", "Pass")->SetPasswordCharacter('*');

	AddLabel("Server", WHITE);

	str lServerName;
	v_get(lServerName, =, mLoginObserver->GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, "localhost:16650");
	if (strutil::StartsWith(lServerName, "0.0.0.0"))
	{
		lServerName = lServerName.substr(7);
	}
	AddTextField(lServerName, "Server");

	AddButton("Login", 0)->SetOnClick(LoginView, OnLogin);

	UpdateLayout();
}



void LoginView::OnExit()
{
	mLoginObserver->CancelLogin();
}

void LoginView::OnLogin(UiTbc::Button*)
{
	// Save user's login info.
	str lServer = ((UiTbc::TextField*)GetChild("Server", 0))->GetText();

	// Pick strings from UI.
	wstr lUsername = wstrutil::Encode(((UiTbc::TextField*)GetChild("User", 0))->GetText());
	UiTbc::TextField* lPasswordComponent = (UiTbc::TextField*)GetChild("Pass", 0);
	wstr lReadablePassword = wstrutil::Encode(lPasswordComponent->GetText());
	lPasswordComponent->SetText("?");	// Clear out password traces in component.

	// Convert into login format.
	Cure::MangledPassword lPassword(lReadablePassword);
	lReadablePassword.clear();	// Clear out password traces in string.
	Cure::LoginId lLoginToken(lUsername, lPassword);

	mLoginObserver->RequestLogin(lServer, lLoginToken);
}



}
