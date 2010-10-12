
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "ServerListView.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/UserAccount.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/Number.h"
#include "../../UiTBC/Include/GUI/UiCaption.h"
#include "../../UiTBC/Include/GUI/UiCenterLayout.h"
#include "../../UiTBC/Include/GUI/UiTextField.h"
#include "RtVar.h"



namespace Life
{



ServerListView::ServerListView(ServerSelectObserver* pSelectObserver):
	View(_T("Join Online Game"), new UiTbc::GridLayout(11, 1)),
	mSelectObserver(pSelectObserver),
	mIsMasterConnectError(false)
{
	mServerList.push_back(ServerInfo());

	SetPreferredSize(500, 350);

	UiTbc::Component* lRowLayer = AddRow(Color(0, 0, 0), 4);
	AddLabel(_T("Server Name"), WHITE, 250, lRowLayer);
	AddLabel(_T("Players"), WHITE, 0, lRowLayer);
	AddLabel(_T("Ping (ms)"), WHITE, 0, lRowLayer);
	AddLabel(_T("Select"), WHITE, 0, lRowLayer);

	CreateLayer(new UiTbc::CenterLayout);
}

void ServerListView::Tick()
{
	// TRICKY! Gaah! Code is worse than it looks.
	if (!mSelectObserver->UpdateServerList(mServerList))
	{
		if (!mIsMasterConnectError && mSelectObserver->IsMasterServerConnectError())
		{
			mIsMasterConnectError = true;
			mServerList.clear();
			ReplaceLayer(1, new UiTbc::CenterLayout);
			AddLabel(_T("Problem connecting to master server.\nCheck your network cable and/or firewall settings."), RED, 0, this, 1)->SetPreferredSize(0, 0, true);
		}
		if (mSelectObserver->IsMasterServerConnectError() == mIsMasterConnectError)
		{
			return;
		}
	}
	if (!mSelectObserver->IsMasterServerConnectError())
	{
		mIsMasterConnectError = false;

		if (mServerList.size() == 0)	// No response yet!
		{
			ReplaceLayer(1, new UiTbc::CenterLayout);
			AddLabel(_T("Please wait while refreshing list..."), GRAY, 0, this, 1)->SetAdaptive(true);
		}
		else if (mServerList.size() == 1)	// Only the status message, no servers online!
		{
			ReplaceLayer(1, new UiTbc::CenterLayout);
			AddLabel(_T("I'm sorry, but no-one else seems to be running a public server.\n")
				_T("Perhaps you would like to be the first to start one? Click here and there..."), GRAY, 0, this, 1)->SetPreferredSize(0, 0, true);
		}
		else if (mServerList.size() > 1)
		{
			DeleteLayer(1);
		}
	}

	const int lServerCount = (int)mServerList.size() - 1;	// Ignore the status message.

	const UiTbc::GridLayout* lGrid = (UiTbc::GridLayout*)GetClientRectComponent()->GetLayout();
	for (int x = 1; x < 1+10; ++x)
	{
		UiTbc::RectComponent* lRowLayer = (UiTbc::RectComponent*)lGrid->GetComponentAt(x, 0);
		if (lRowLayer)
		{
			lRowLayer->DeleteChildrenInLayer(0);
		}
		else
		{
			lRowLayer = AddRow(DARK_GRAY, 4);
		}
		const bool lMoreServers = (x-1 < lServerCount);
		if (!lMoreServers)
		{
			lRowLayer->SetIsHollow(true);
		}
		else if (x&1)
		{
			lRowLayer->SetColor(Color(90, 90, 90));
			lRowLayer->SetIsHollow(false);
		}
		else
		{
			lRowLayer->SetColor(Color(96, 96, 96));
			lRowLayer->SetIsHollow(false);
		}
		if (lMoreServers)
		{
			const ServerInfo& lServer = mServerList[x-1];
			AddLabel(lServer.mName, GRAY, 250, lRowLayer)->SetAdaptive(false);
			AddLabel(strutil::IntToString(lServer.mPlayerCount, 10), GRAY, 0, lRowLayer);
			const str lPing = (lServer.mPing > 0)? Number::ConvertToPostfixNumber(lServer.mPing, 0)+_T("s") : _T("");
			AddLabel(lPing, GRAY, 0, lRowLayer);
			UiTbc::RectComponent* lCenterLayer = AddCentering(0, lRowLayer);
			UiTbc::Button* lButton = AddButton(_T("Pick"), (void*)(x-1), lCenterLayer);
			lButton->SetOnClick(ServerListView, OnSelect);
			lButton->SetPreferredSize(100, 23, false);
		}
	}

	GetClientRectComponent()->UpdateLayout();
	//UpdateLayout();	TRICKY!
}



void ServerListView::OnExit()
{
	mSelectObserver->OnCancelJoinServer();
}

void ServerListView::OnSelect(UiTbc::Button* pButton)
{
	const size_t lServerIndex = (intptr_t)pButton->GetExtraData();
	if (lServerIndex < mServerList.size())
	{
		mSelectObserver->OnRequestJoinServer(mServerList[lServerIndex].mGivenIpAddress +
			strutil::Format(_T(":%i"), mServerList[lServerIndex].mGivenPort));
	}
}



}
