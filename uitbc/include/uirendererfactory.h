
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../uilepra/include/uidisplaymanager.h"
#include "uirenderer.h"



namespace uitbc {



class RendererFactory {
public:
	static Renderer* Create(uilepra::DisplayManager::ContextType type, Canvas* screen);
};



}
