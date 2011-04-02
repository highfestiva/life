
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../RaceScore.h"



namespace UiCure
{
class GameUiManager;
}



namespace Life
{



class ScreenPart;



class UiRaceScore: public RaceScore
{
	typedef RaceScore Parent;
public:
	UiRaceScore(Cure::ContextObject* pContextObject, const str& pAttributeName, ScreenPart* pScreenPart,
		UiCure::GameUiManager* pUiManager, const str& pClassResourceName);
	virtual ~UiRaceScore();

private:
	Cure::GameObjectId mStopWatchId;
};



}
