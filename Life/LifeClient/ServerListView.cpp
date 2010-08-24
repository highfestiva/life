
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "ServerListView.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/UserAccount.h"
#include "../../Lepra/Include/Network.h"
#include "../../UiTBC/Include/GUI/UiCaption.h"
#include "../../UiTBC/Include/GUI/UiTextField.h"
#include "RtVar.h"



namespace Life
{



ServerListView::ServerListView(ServerSelectObserver* pSelectObserver):
	View(_T("Login"), new UiTbc::GridLayout(10, 3)),
	mSelectObserver(pSelectObserver)
{
	SetPreferredSize(500, 350);

	AddLabel(_T("Server name"), WHITE);
	AddLabel(_T("ABC"), GRAY);

	UpdateLayout();
}



void ServerListView::OnExit()
{
	mSelectObserver->CancelJoinServer();
}

void ServerListView::OnSelect(UiTbc::Button*)
{
	str lServer = ((UiTbc::TextField*)GetChild(_T("Server"), 0))->GetText();
	mSelectObserver->RequestJoinServer(lServer);
}



}
