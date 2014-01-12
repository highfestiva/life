
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
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
#ifdef LEPRA_WINDOWS
#define EXTERN_HINSTANCE	extern HINSTANCE ghInstance
#define FETCH_HINSTANCE()	ghInstance = (HINSTANCE)::GetModuleHandle(0)
#else // !Win
#define EXTERN_HINSTANCE
#define FETCH_HINSTANCE()
#endif // Win32/Other.
#define LEPRA_RUN_APPLICATION(ClassName, Method)	\
EXTERN_HINSTANCE;	\
int main(int argc, const char* argv[])	\
{	\
	FETCH_HINSTANCE();	\
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
#ifdef LEPRA_UNICODE
#define LEPRA_WINMAIN wWinMain
#else // ANSI
#define LEPRA_WINMAIN WinMain
#endif // Unicode/ANSI
#define LEPRA_RUN_APPLICATION(ClassName, Method)	\
extern HINSTANCE ghInstance;	\
int WINAPI LEPRA_WINMAIN(HINSTANCE hInstance, HINSTANCE, PTSTR, int)	\
{	\
	ghInstance = hInstance;	\
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
	virtual void Destroy() = 0;

	virtual int Run() = 0;

	virtual void Suspend();
	virtual void Resume();
};



int Main(Application& pApplication);



}
