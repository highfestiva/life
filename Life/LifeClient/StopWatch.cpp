
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "StopWatch.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../../UiTbc/Include/GUI/UiFloatingLayout.h"
#include "../RaceScore.h"
#include "ScreenPart.h"



namespace Life
{



StopWatch::StopWatch(ScreenPart* pScreenPart, UiCure::GameUiManager* pUiManager, const str& pClassResourceName,
	const str& pAttributeName):
	Parent(0, pClassResourceName, pUiManager),
	mScreenPart(pScreenPart),
	mAttributeName(pAttributeName),
	mLabel(RED, L"")
{
	mLabel.SetPreferredSize(50, 20);
	mLabel.SetMinSize(30, 15);
	mLabel.SetPos(0, 0);
	GetUiManager()->AssertDesktopLayout(new UiTbc::FloatingLayout, 0);
	GetUiManager()->GetDesktopWindow()->AddChild(&mLabel, 0, 0, 0);
}

StopWatch::~StopWatch()
{
	GetUiManager()->GetDesktopWindow()->RemoveChild(&mLabel, 0);
}

void StopWatch::Start(Cure::ContextObject* pParent)
{
	pParent->GetManager()->AddLocalObject(this);
	pParent->AddChild(this);
	GetManager()->EnableTickCallback(this);
}



void StopWatch::OnTick()
{
	if (!GetManager())
	{
		return;
	}

	const RaceScore* lScore = (RaceScore*)mParent->GetAttribute(mAttributeName);
	if (!lScore)
	{
		GetManager()->PostKillObject(GetInstanceId());
		return;
	}
	PixelRect lArea = mScreenPart->GetRenderArea();
	mLabel.SetPos(lArea.mRight-65, lArea.mTop+16);
	const double lTime = lScore->GetTime();
	const int lMinute = (int)(lTime / 60);
	const int lSecond = (int)::fmod(lTime, 60);
	const int lCentiSecond = (int)(100 * (lTime - lMinute*60 - lSecond));
	mLabel.SetText(wstrutil::Format(L"%i:%2.2i.%2.2i", lMinute, lSecond, lCentiSecond));
}



loginstance(GAME_CONTEXT_CPP, StopWatch);



}
