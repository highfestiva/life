
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "UiRaceScore.h"
#include "../../Cure/Include/ContextManager.h"
#include "StopWatch.h"



namespace Life
{



UiRaceScore::UiRaceScore(Cure::ContextObject* pContextObject, const str& pAttributeName, ScreenPart* pScreenPart,
	UiCure::GameUiManager* pUiManager, const str& pClassResourceName):
	Parent(pContextObject, pAttributeName, 0, TBC::INVALID_TRIGGER)
{
	StopWatch* lStopWatchChild = new StopWatch(pScreenPart, pUiManager, pClassResourceName, pAttributeName);
	lStopWatchChild->Start(pContextObject);
}

UiRaceScore::~UiRaceScore()
{
}



}
