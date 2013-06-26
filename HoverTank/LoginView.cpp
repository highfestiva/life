
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "LoginView.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Cure/Include/UserAccount.h"
#include "../Lepra/Include/Network.h"
#include "../UiTBC/Include/GUI/UiCaption.h"
#include "../UiTBC/Include/GUI/UiTextField.h"
#include "RtVar.h"



namespace HoverTank
{



LoginView::LoginView(ClientLoginObserver* pLoginObserver, const str& pErrorMessage):
	View(_T("Login"), new UiTbc::GridLayout(7, 1)),
	mLoginObserver(pLoginObserver)
{
	SetPreferredSize(200, 200);

	if (!pErrorMessage.empty())
	{
		((UiTbc::GridLayout*)GetClientRectComponent()->GetLayout())->InsertRow(0);
		AddLabel(pErrorMessage, RED);
	}

	AddLabel(_T("Username"), WHITE);

	str lUserName;
	CURE_RTVAR_TRYGET(lUserName, =, mLoginObserver->GetVariableScope(), RTVAR_LOGIN_USERNAME, _T("User0"));
	AddTextField(lUserName, _T("User"));

	AddLabel(_T("Password"), WHITE);

	AddTextField(_T("CarPassword"), _T("Pass"))->SetPasswordCharacter(_T('*'));

	AddLabel(_T("Server"), WHITE);

	str lServerName;
	CURE_RTVAR_GET(lServerName, =, mLoginObserver->GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, _T("localhost:16650"));
	if (strutil::StartsWith(lServerName, _T("0.0.0.0")))
	{
		lServerName = lServerName.substr(7);
	}
	AddTextField(lServerName, _T("Server"));

	AddButton(_T("Login"), 0)->SetOnClick(LoginView, OnLogin);

	UpdateLayout();
}



void LoginView::OnExit()
{
	mLoginObserver->CancelLogin();
}

void LoginView::OnLogin(UiTbc::Button*)
{
	// Save user's login info.
	str lServer = ((UiTbc::TextField*)GetChild(_T("Server"), 0))->GetText();

	// Pick strings from UI.
	wstr lUsername = wstrutil::Encode(((UiTbc::TextField*)GetChild(_T("User"), 0))->GetText());
	UiTbc::TextField* lPasswordComponent = (UiTbc::TextField*)GetChild(_T("Pass"), 0);
	wstr lReadablePassword = wstrutil::Encode(lPasswordComponent->GetText());
	lPasswordComponent->SetText(_T("?"));	// Clear out password traces in component.

	// Convert into login format.
	Cure::MangledPassword lPassword(lReadablePassword);
	lReadablePassword.clear();	// Clear out password traces in string.
	Cure::LoginId lLoginToken(lUsername, lPassword);

	mLoginObserver->RequestLogin(lServer, lLoginToken);
}



}
