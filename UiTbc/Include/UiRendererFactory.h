
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#ifndef UIRENDERERFACTORY_H
#define UIRENDERERFACTORY_H



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



#endif // !UIRENDERERFACTORY_H