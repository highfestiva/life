
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/lepraos.h"
#include "../include/application.h"
#include "../include/systemmanager.h"



#ifdef LEPRA_WINDOWS
HINSTANCE ghInstance = 0;
#endif // Windows



namespace lepra {



Application::Application(const strutil::strvec& argument_vector) {
	SystemManager::SetArgumentVector(argument_vector);
}

Application::~Application() {
}

void Application::Resume(bool hard) {
	(void)hard;
}

void Application::Suspend(bool hard) {
	(void)hard;
}



int Main(Application& application) {
	application.Init();
	return application.Run();
}



}
