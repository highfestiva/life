/*
	Class:  DllLoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	A class used to simplify dll-file loading at runtime.
*/

#ifndef DLLLOADER_H
#define DLLLOADER_H

#include "Lepra.h"

namespace Lepra
{

class DllLoader
{
public:
	// Returns the handle to the opened library. NULL on failure.
	static void* LoadDLL(const tchar* pFileName,
			     int pNumFuncs,
			     ...);
protected:
private:
};

} // End namespace.

#endif // DLLLOADER_H
