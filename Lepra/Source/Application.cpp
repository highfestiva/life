
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Application.h"



HINSTANCE ghInstance = 0;



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
