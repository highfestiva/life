
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiTBC/Include/GUI/UiLabel.h"
#include "../Life.h"



namespace Life
{



class ScreenPart;



class StopWatch: public UiCure::CppContextObject
{
	typedef UiCure::CppContextObject Parent;
public:
	StopWatch(ScreenPart* pScreenPart, UiCure::GameUiManager* pUiManager, const str& pClassResourceName,
		const str& pAttributeName);
	virtual ~StopWatch();
	void Start(Cure::ContextObject* pParent);

protected:
	void OnTick();

private:
	ScreenPart* mScreenPart;
	str mAttributeName;
	UiTbc::Label mLabel;

	LOG_CLASS_DECLARE();
};



}
