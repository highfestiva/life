
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



namespace Push
{



LoginView::LoginView(ClientLoginObserver* pLoginObserver, const wstr& pErrorMessage):
	View(L"Login", new UiTbc::GridLayout(7, 1)),
	mLoginObserver(pLoginObserver)
{
	SetPreferredSize(200, 200);

	if (!pErrorMessage.empty())
	{
		((UiTbc::GridLayout*)GetClientRectComponent()->GetLayout())->InsertRow(0);
		AddLabel(pErrorMessage, RED);
	}

	AddLabel(L"Username", WHITE);

	str lUserName;
	v_tryget(lUserName, =, mLoginObserver->GetVariableScope(), RTVAR_LOGIN_USERNAME, "User0");
	AddTextField(wstrutil::Encode(lUserName), "User");

	AddLabel(L"Password", WHITE);

	AddTextField(L"CarPassword", "Pass")->SetPasswordCharacter('*');

	AddLabel(L"Server", WHITE);

	str lServerName;
	v_get(lServerName, =, mLoginObserver->GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, "localhost:16650");
	if (strutil::StartsWith(lServerName, "0.0.0.0"))
	{
		lServerName = lServerName.substr(7);
	}
	AddTextField(wstrutil::Encode(lServerName), "Server");

	AddButton(L"Login", 0)->SetOnClick(LoginView, OnLogin);

	UpdateLayout();
}



void LoginView::OnExit()
{
	mLoginObserver->CancelLogin();
}

void LoginView::OnLogin(UiTbc::Button*)
{
	// Save user's login info.
	str lServer = strutil::Encode(((UiTbc::TextField*)GetChild("Server"))->GetText());

	// Pick strings from UI.
	str lUsername = strutil::Encode(((UiTbc::TextField*)GetChild("User"))->GetText());
	UiTbc::TextField* lPasswordComponent = (UiTbc::TextField*)GetChild("Pass");
	str lReadablePassword = strutil::Encode(lPasswordComponent->GetText());
	lPasswordComponent->SetText(L"?");	// Clear out password traces in component.

	// Convert into login format.
	Cure::MangledPassword lPassword(lReadablePassword);
	lReadablePassword.clear();	// Clear out password traces in string.
	Cure::LoginId lLoginToken(lUsername, lPassword);

	mLoginObserver->RequestLogin(lServer, lLoginToken);
}



}
