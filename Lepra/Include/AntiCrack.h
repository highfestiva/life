
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0510
#endif // _WIN32_WINNT
#include "LepraTarget.h"
#include "LepraTypes.h"

#pragma once



#define AntiCrack			sY
#define SetBsodOnProcessExit		s_pa_q_
#define EnableCriticalPrivileges	Ep



// Left in global namespace with purpose.



class AntiCrack
{
public:
	AntiCrack();

#ifdef LEPRA_WINDOWS
	void SetBsodOnProcessExit();
	bool EnableCriticalPrivileges(LPCTSTR pPrivilegeName);
#endif // Windows
};
