
// Author: Jonas Bystr�m
// Copyright (c) 2002-2008, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/UserAccount.h"
#include "../../Lepra/Include/Network.h"
#include "../../UiTbc/Include/GUI/UiCaption.h"
#include "../../UiTbc/Include/GUI/UiTextField.h"
#include "ClientLoginView.h"



namespace Life
{



ClientLoginView::ClientLoginView(UiTbc::Painter* pPainter, ClientLoginObserver* pLoginObserver, const Lepra::String& pErrorMessage):
	View(pPainter, _T("Login"), new UiTbc::GridLayout(7, 1)),
	mLoginObserver(pLoginObserver)
{
	SetPreferredSize(200, 200);

	if (!pErrorMessage.empty())
	{
		((UiTbc::GridLayout*)GetClientRectComponent()->GetLayout())->InsertRow(0);
		AddLabel(pErrorMessage, Lepra::RED);
	}

	AddLabel(_T("Username"), Lepra::WHITE);

	Lepra::String lUserName = CURE_RTVAR_GETSET(mLoginObserver->GetVariableScope(), "Login.UserName", _T("Car0"));
	AddTextField(lUserName, _T("User"));

	AddLabel(_T("Password"), Lepra::WHITE);

	AddTextField(_T("CarPassword"), _T("Pass"))->SetPasswordCharacter(_T('*'));

	AddLabel(_T("Server"), Lepra::WHITE);

	Lepra::String lServerName;
	{
		Lepra::IPAddress lLocalIp;
		if (Lepra::Network::ResolveHostname(_T(""), lLocalIp))
		{
			lServerName = lLocalIp.GetAsString();
		}
		else
		{
			lServerName = _T("localhost");
		}
		lServerName += _T(":16650");
		CURE_RTVAR_GETSET(mLoginObserver->GetVariableScope(), "Login.DefaultServer", lServerName);
	}
	AddTextField(lServerName, _T("Server"));

	AddButton(_T("Login"))->SetOnUnclickedFunc(ClientLoginView, OnLogin);

	UpdateLayout();
}



void ClientLoginView::OnExit()
{
	mLoginObserver->CancelLogin();
}

void ClientLoginView::OnLogin(UiTbc::Button*)
{
	// Save user's login info.
	CURE_RTVAR_SET(mLoginObserver->GetVariableScope(), "Login.UserName", ((UiTbc::TextField*)GetChild(_T("User"), 0))->GetText());
	Lepra::String lServer = ((UiTbc::TextField*)GetChild(_T("Server"), 0))->GetText();
	CURE_RTVAR_SET(mLoginObserver->GetVariableScope(), "Login.DefaultServer", lServer);

	// Pick strings from UI.
	Lepra::UnicodeString lUsername = Lepra::UnicodeStringUtility::ToOwnCode(((UiTbc::TextField*)GetChild(_T("User"), 0))->GetText());
	UiTbc::TextField* lPasswordComponent = (UiTbc::TextField*)GetChild(_T("Pass"), 0);
	Lepra::UnicodeString lReadablePassword = Lepra::UnicodeStringUtility::ToOwnCode(lPasswordComponent->GetText());
	lPasswordComponent->SetText(_T("?"));	// Clear out password traces in component.

	// Convert into login format.
	Cure::MangledPassword lPassword(lReadablePassword);
	lReadablePassword.clear();	// Clear out password traces in string.
	Cure::LoginId lLoginToken(lUsername, lPassword);

	mLoginObserver->RequestLogin(lServer, lLoginToken);
}



}
