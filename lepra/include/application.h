
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
//
// The Application class handles the program arguments
// and keeps the application from being platform specific,
// mainly by handling both console and window applications
// on Windows systems.



#include "lepratarget.h"
#include "string.h"



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
	lepra::strutil::strvec arguments;	\
	for (int x = 0; x < argc; ++x)	\
	{	\
		arguments.push_back(lepra::str(argv[x]));	\
	}	\
	ClassName* _application = new ClassName(arguments);	\
	const int status = Method(*_application);	\
	delete _application;	\
	return status;	\
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
	lepra::strutil::strvec arguments(lepra::strutil::BlockSplit(::GetCommandLine(), " \t\v\r\n", false, false));	\
	ClassName* _application = new ClassName(arguments);	\
	const int status = Method(*_application);	\
	delete _application;	\
	return status;	\
}
#endif // LEPRA_CONSOLE/!LEPRA_CONSOLE



namespace lepra {



class Application {
public:
	Application(const strutil::strvec& argument_vector);
	virtual ~Application();
	virtual void Init() = 0;

	virtual int Run() = 0;

	virtual void Resume(bool hard);
	virtual void Suspend(bool hard);
};



int Main(Application& application);



}
