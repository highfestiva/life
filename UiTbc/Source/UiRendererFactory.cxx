
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiRendererFactory.h"
//#include "../Include/UiDirectX9Renderer.h"	// TODO: port!
#include "../Include/UiOpenGLRenderer.h"



namespace UiTbc
{



Renderer* RendererFactory::Create(UiLepra::DisplayManager::ContextType pType, Canvas* pScreen)
{
	Renderer* lRenderer = 0;
	switch (pType)
	{
		case UiLepra::DisplayManager::DIRECTX_CONTEXT:	/*lRenderer = new DirectX9Renderer(pScreen);*/	break;
		case UiLepra::DisplayManager::OPENGL_CONTEXT:	lRenderer = new OpenGLRenderer(pScreen);	break;
	}
	return (lRenderer);
}



}
