
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/LepraOS.h"
#include "../Include/Application.h"
#include "../Include/SystemManager.h"



#ifdef LEPRA_WINDOWS
HINSTANCE ghInstance = 0;
#endif // Windows



namespace Lepra
{



Application::Application(const strutil::strvec& pArgumentVector)
{
	SystemManager::SetArgumentVector(pArgumentVector);
}

Application::~Application()
{
}

void Application::Suspend()
{
}

void Application::Resume()
{
}



int Main(Application& pApplication)
{
	pApplication.Init();
	return pApplication.Run();
}



}
