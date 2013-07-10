
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/Application.h"



#ifdef LEPRA_WINDOWS
HINSTANCE ghInstance = 0;
#endif // Windows



namespace Lepra
{



Application::Application(const strutil::strvec& pArgumentVector):
	mArgumentVector(pArgumentVector)
{
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
