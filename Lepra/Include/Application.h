
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games
//
// The Application class handles the program arguments
// and keeps the application from being platform specific,
// mainly by handling both console and window applications
// on Windows systems.



#include "LepraTarget.h"
#include "String.h"



#pragma once



// TRICKY: implemented this code in macros, since it is target specific.
#ifdef LEPRA_CONSOLE
#define LEPRA_RUN_APPLICATION(ClassName, Method)	\
int main(int argc, const char* argv[])	\
{	\
	Lepra::strutil::strvec lArguments;	\
	for (int x = 0; x < argc; ++x)	\
	{	\
		lArguments.push_back(Lepra::strutil::Encode(Lepra::astr(argv[x])));	\
	}	\
	ClassName* lApplication = new ClassName(lArguments);	\
	const int lStatus = Method(*lApplication);	\
	delete lApplication;	\
	return lStatus;	\
}
#else // !LEPRA_CONSOLE (Let's assume it's M$Win, shall we?)
#define LEPRA_RUN_APPLICATION(ClassName, Method)	\
int WINAPI wWinMain(HINSTANCE, HINSTANCE, PTSTR, int)	\
{	\
	Lepra::strutil::strvec lArguments(Lepra::strutil::BlockSplit(::GetCommandLine(), _T(" \t\v\r\n"), false, false));	\
	ClassName* lApplication = new ClassName(lArguments);	\
	const int lStatus = Method(*lApplication);	\
	delete lApplication;	\
	return lStatus;	\
}
#endif // LEPRA_CONSOLE/!LEPRA_CONSOLE



namespace Lepra
{



class Application
{
public:
	Application(const strutil::strvec& pArgumentVector);
	virtual ~Application();
	virtual void Init() = 0;

	virtual int Run() = 0;

protected:
	strutil::strvec mArgumentVector;
};



int Main(Application& pApplication);



}
