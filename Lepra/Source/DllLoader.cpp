/*
	Class:  DllLoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../Include/DllLoader.h"

#include <stdarg.h>

#ifdef LEPRA_POSIX
#include <dlfcn.h>
#endif

namespace Lepra
{

#ifdef LEPRA_WINDOWS
void* DllLoader::LoadDLL(const tchar* pFileName,
			 int pNumFuncs,
			 ...)
{
	HINSTANCE lLibrary = NULL;

	va_list lArg;			// Pointer to the current argument.
	va_start(lArg, pNumFuncs);	// Set lArg to point at the first argument.
	
	if ((lLibrary = LoadLibrary(pFileName)) != NULL)
	{
		FARPROC* lFunctionPtr = NULL;
		const char* lFuncName = NULL;

		int lCount = 0;
		while (lCount < pNumFuncs)
		{
			lFunctionPtr  = va_arg(lArg, FARPROC*);
			lFuncName    = va_arg(lArg, const char*);

			if ((*lFunctionPtr = GetProcAddress(lLibrary, lFuncName)) == NULL)
			{
				lFunctionPtr = NULL;
				// TODO: Close library.
				lLibrary = NULL;
				break;
			}

			lCount++;
		}
	}

	va_end(lArg);
	return (void*)lLibrary;
}

#elif defined LEPRA_POSIX

// TODO: Verify that this function works.

void* DllLoader::LoadDLL(const tchar* pFileName,
			 int pNumFuncs,
			 ...)
{
	void* lLibrary = NULL;
	bool lSuccess = true;

	va_list lArg;			// Pointer to the current argument.
	va_start(lArg, pNumFuncs);	// Set lArg to point at the first argument.
	
	if ((lLibrary = dlopen(pFileName, RTLD_LAZY | RTLD_LOCAL)) != NULL)
	{
		typedef int (*funcPtr)(int);
		funcPtr* lFunctionPtr = NULL;
		const char* lFuncName = NULL;

		int lCount = 0;
		while (lCount < pNumFuncs)
		{
			lFunctionPtr  = va_arg(lArg, funcPtr*);
			lFuncName    = va_arg(lArg, const char*);

			if ((*lFunctionPtr = (funcPtr)dlsym( lLibrary, lFuncName )) == NULL)
			{
				lFunctionPtr = NULL;
				lLibrary = NULL;
				break;
			}

			lCount++;
		}
	}

	va_end(lArg);
	return lLibrary;
}

#endif // LEPRA_WINDOWS/LEPRA_POSIX

} // End namespace.
