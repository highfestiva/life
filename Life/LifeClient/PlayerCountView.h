
// Author: Jonas Bystr�m
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "View.h"



namespace Life
{



class PlayerCountObserver;



class PlayerCountView: public View
{
public:
	PlayerCountView(UiTbc::Painter* pPainter, PlayerCountObserver* pPlayerCountObserver);

private:
	void OnExit();
	void OnClick(UiTbc::Button*, int pIndex);

	PlayerCountObserver* mPlayerCountObserver;
};



class PlayerCountObserver
{
public:
	virtual void OnExit(View* pPlayerCountView) = 0;
	virtual void OnSetPlayerCount(View* pPlayerCountView, int pPlayerCount) = 0;
};



}
