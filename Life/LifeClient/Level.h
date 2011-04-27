
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "Machine.h"



namespace Life
{



class Level: public Machine
{
public:
	typedef Machine Parent;

	Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Level();

	LOG_CLASS_DECLARE();
};



}
