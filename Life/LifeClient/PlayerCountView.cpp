
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Network.h"
#include "../../UiTBC/Include/GUI/UiCaption.h"
#include "../../UiTBC/Include/GUI/UiTextField.h"
#include "PlayerCountView.h"



namespace Life
{



PlayerCountView::PlayerCountView(PlayerCountObserver* pPlayerCountObserver):
	View(_T("Splitscreen player count"), new UiTbc::GridLayout(4, 1)),
	mPlayerCountObserver(pPlayerCountObserver)
{
	SetPreferredSize(200, 300);

	AddButton(_T("Single player"), (void*)1)->SetOnClick(PlayerCountView, OnClick);
	AddButton(_T("Two players"), (void*)2)->SetOnClick(PlayerCountView, OnClick);
	AddButton(_T("Three players"), (void*)3)->SetOnClick(PlayerCountView, OnClick);
	AddButton(_T("Four players"), (void*)4)->SetOnClick(PlayerCountView, OnClick);

	UpdateLayout();
}



void PlayerCountView::OnExit()
{
	mPlayerCountObserver->OnExit();
}

void PlayerCountView::OnClick(UiTbc::Button* pButton)
{
	mPlayerCountObserver->OnSetPlayerCount((int)(intptr_t)pButton->GetExtraData());
}



}
