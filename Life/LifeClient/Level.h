
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Lepra/Include/HiResTimer.h"
#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiTBC/Include/UiChunkyClass.h"
#include "../Life.h"



namespace Life
{



class Level: public UiCure::CppContextObject
{
public:
	typedef UiCure::CppContextObject Parent;

	Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Level();

	LOG_CLASS_DECLARE();
};



}
