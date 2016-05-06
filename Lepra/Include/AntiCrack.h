
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0510
#endif // _WIN32_WINNT
#include "lepraos.h"

#pragma once



#define AntiCrack			sY
#define SetBsodOnProcessExit		s_pa_q_
#define EnableCriticalPrivileges	Ep



// Left in global namespace with purpose.



class AntiCrack {
public:
	AntiCrack();

#ifdef LEPRA_WINDOWS
	void SetBsodOnProcessExit();
	bool EnableCriticalPrivileges(LPCTSTR privilege_name);
#endif // Windows
};
