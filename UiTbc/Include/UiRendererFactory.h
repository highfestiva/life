
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../UiLepra/Include/UiDisplayManager.h"
#include "UiRenderer.h"



namespace UiTbc
{



class RendererFactory
{
public:
	static Renderer* Create(UiLepra::DisplayManager::ContextType pType, Canvas* pScreen);
};



}
