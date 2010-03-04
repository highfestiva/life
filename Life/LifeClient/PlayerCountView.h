
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "View.h"



namespace Life
{



class PlayerCountObserver;



class PlayerCountView: public View
{
public:
	PlayerCountView(PlayerCountObserver* pPlayerCountObserver);

private:
	void OnExit();
	void OnClick(UiTbc::Button*);

	PlayerCountObserver* mPlayerCountObserver;
};



class PlayerCountObserver
{
public:
	virtual void OnExit() = 0;
	virtual void OnSetPlayerCount(int pPlayerCount) = 0;
};



}
