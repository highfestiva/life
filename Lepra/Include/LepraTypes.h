
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "LepraTarget.h"




namespace Lepra
{



#ifdef LEPRA_MSVC_X86
typedef signed char		int8;
typedef unsigned char		uint8;
typedef signed short		int16;
typedef unsigned short		uint16;
typedef signed			int32;
typedef unsigned		uint32;
typedef __int64			int64;
typedef unsigned __int64	uint64;
typedef float			float32;
typedef double			float64;
#elif defined(LEPRA_MSVC_ARM)
typedef signed char		int8;
typedef unsigned char		uint8;
typedef signed short		int16;
typedef unsigned short		uint16;
typedef signed			int32;
typedef unsigned		uint32;
typedef __int64			int64;
typedef unsigned __int64	uint64;
typedef	uint16			wchar_t;
typedef float			float32;
typedef double			float64;
#elif defined(LEPRA_GCC_X86)
typedef signed char		int8;
typedef unsigned char		uint8;
typedef signed short		int16;
typedef unsigned short		uint16;
typedef signed			int32;
typedef unsigned		uint32;
typedef long long		int64;
typedef unsigned long long	uint64;
typedef float			float32;
typedef double			float64;
#else	// Unknown platform
#error "Hardware platform not supported! Define LEPRA_MSVC_X86/LEPRA_MSVC_ARM/LEPRA_GCC_X86/etc to compile for that platform."
#endif	// Platform

#if defined(LEPRA_MSVC) && !defined(_NATIVE_WCHAR_T_DEFINED)
typedef unsigned short	wchar_t;
#endif // LEPRA_MSVC && !_NATIVE_WCHAR_T_DEFINED
#ifdef	LEPRA_UNICODE
typedef	signed short		stchar;
typedef	wchar_t			utchar;
typedef	wchar_t			tchar;
#ifdef	__T
#undef __T
#endif // __T
#define	__T(x)			L ## x
#define _TEXT_ALTERNATIVE(x, y)	y
#else	// !LEPRA_UNICODE
typedef signed char		stchar;
typedef unsigned char		utchar;
typedef char			tchar;
#ifdef	__T
#undef __T
#endif // __T
#define	__T(x)			x
#define _TEXT_ALTERNATIVE(x, y)	x
#endif	// LEPRA_UNICODE/!LEPRA_UNICODE
#ifdef	_T
#undef _T
#endif // _T
// Two-step macro (__T and _T) allows for resolving symbolic constants.
#define	_T(x)			__T(x)



}
