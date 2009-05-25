
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2008, Righteous Games

// TRICKY: do NOT add "#pragma once" to this file.



#ifndef LEPRA_INCLUDE_NO_OS



#ifndef LEPRA_OS_INCLUDED
#define LEPRA_OS_INCLUDED

#define WIN32_LEAN_AND_MEAN

#pragma warning(push)	// Disable warnings for M$ header files.
#pragma warning(disable: 4201)	// Nonstandard extention: unnamed struct.

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0500
#endif // _WIN32_WINNT

#include <Windows.h>
#include <WindowsX.h>
#ifdef LEPRA_NETWORK_IPV6
#include <Ws2tcpip.h>
#endif // LEPRA_NETWORK_IPV6
#include <WinSock.h>

#pragma warning(pop)	// Enable all warnings again.

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x80000 
#endif // !WS_EX_LAYERED

// Drop asshole M$ #defines.
#include "../LepraUndef.h"

#if _MSC_VER > 1310	// <MSVS 2005+>
#define WIDE_SPRINTF(buf, size, fmt, arg)	::vswprintf_s(buf, size, fmt, arg)
#else // <MSVS 2003 .NET->
#define WIDE_SPRINTF(buf, size, fmt, arg)	::vswprintf(buf, fmt, arg)
#endif // <MSVS 2005+> / <MSVS 2003 .NET->

#define WIDE_ATOI(str)				::_wtoi(str)

#define SOCKET_LAST_ERROR()			::WSAGetLastError()

namespace Lepra
{
	// OS types.
	typedef	SOCKET sys_socket;
	typedef int socklen_t;
}

#endif // !LEPRA_OS_INCLUDED



#else // LEPRA_INCLUDE_NO_OS



#if !defined(LEPRA_INTPTR_T) && !defined(LEPRA_OS_INCLUDED)
#define LEPRA_INTPTR_T
typedef __w64 int intptr_t;
#endif // !LEPRA_INTPTR_T+!



#endif // !LEPRA_INCLUDE_NO_OS/LEPRA_INCLUDE_NO_OS
