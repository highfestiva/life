
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Network.h"
#include "../../UiTbc/Include/GUI/UiCaption.h"
#include "../../UiTbc/Include/GUI/UiTextField.h"
#include "PlayerCountView.h"



namespace Life
{



PlayerCountView::PlayerCountView(UiTbc::Painter* pPainter, PlayerCountObserver* pPlayerCountObserver):
	View(pPainter, _T("Splitscreen player count"), new UiTbc::GridLayout(4, 1)),
	mPlayerCountObserver(pPlayerCountObserver)
{
	SetPreferredSize(200, 300);

	AddButton(_T("Single player"))->SetOnUnclickedFuncIndex(PlayerCountView, OnClick, 1);
	AddButton(_T("Two players"))->SetOnUnclickedFuncIndex(PlayerCountView, OnClick, 2);
	AddButton(_T("Three players"))->SetOnUnclickedFuncIndex(PlayerCountView, OnClick, 3);
	AddButton(_T("Four players"))->SetOnUnclickedFuncIndex(PlayerCountView, OnClick, 4);

	UpdateLayout();
}



void PlayerCountView::OnExit()
{
	mPlayerCountObserver->OnExit();
}

void PlayerCountView::OnClick(UiTbc::Button*, int pIndex)
{
	mPlayerCountObserver->OnSetPlayerCount(pIndex);
}



}
