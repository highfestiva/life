
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games
//
// The Application class handles the program arguments
// and keeps the application from being platform specific,
// mainly by handling both console and window applications
// on Windows systems.



#include "StringUtility.h"
#include "LepraTarget.h"



#pragma once



// TRICKY: implemented this code in macros, since it is target specific.
#ifdef LEPRA_CONSOLE
#define LEPRA_RUN_APPLICATION(ClassName)	\
int main(int argc, const char* argv[])	\
{	\
	strutil::strvec lArguments;	\
	for (int x = 0; x < argc; ++x)	\
	{	\
		lArguments.push_back(astrutil::ToCurrentCode(astr(argv[x])));	\
	}	\
	ClassName lApplication(lArguments);	\
	return (lApplication.Run());	\
}
#else // !LEPRA_CONSOLE (Let's assume it's M$Win, shall we?)
#define LEPRA_RUN_APPLICATION(ClassName)	\
int WINAPI wWinMain(HINSTANCE, HINSTANCE, PTSTR, int)	\
{	\
	strutil::strvec lArguments(strutil::BlockSplit(::GetCommandLine(), _T(" \t\v\r\n"), false, false));	\
	ClassName lApplication(lArguments);	\
	return (lApplication.Run());	\
}
#endif // LEPRA_CONSOLE/!LEPRA_CONSOLE



namespace Lepra
{



class Application
{
public:
	Application(const strutil::strvec& pArgumentVector);
	virtual ~Application();

	virtual int Run() = 0;

protected:
	strutil::strvec mArgumentVector;
};



}
