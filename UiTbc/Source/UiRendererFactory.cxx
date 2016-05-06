
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uirendererfactory.h"
//#include "../Include/UiDirectX9Renderer.h"	// TODO: port!
#include "../include/uiopenglrenderer.h"



namespace uitbc {



Renderer* RendererFactory::Create(uilepra::DisplayManager::ContextType type, Canvas* screen) {
	Renderer* renderer = 0;
	switch (type) {
		case uilepra::DisplayManager::kDirectxContext:	/*renderer = new DirectX9Renderer(screen);*/	break;
		case uilepra::DisplayManager::kOpenglContext:	renderer = new OpenGLRenderer(screen);	break;
	}
	return (renderer);
}



}
