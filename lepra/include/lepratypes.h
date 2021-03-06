
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratarget.h"




namespace lepra {



#if defined(LEPRA_MSVC_X86_32) || defined(LEPRA_MSVC_X86_64) || defined(LEPRA_MSVC_ARM)

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
#define LEPRA_LONGLONG(x)	x
#define LEPRA_ULONGLONG(x)	x
#if defined(LEPRA_MSVC) && !defined(_NATIVE_WCHAR_T_DEFINED)
typedef uint16			wchar_t;
#endif // LEPRA_MSVC && !_NATIVE_WCHAR_T_DEFINED

#elif defined(LEPRA_GCC_X86_32) || defined(LEPRA_GCC_X86_64) || defined(LEPRA_GCC_POWERPC) || defined(LEPRA_GCC_ARM)

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
#define LEPRA_LONGLONG(x)	x ## LL
#define LEPRA_ULONGLONG(x)	x ## ULL

#else	// Unknown platform

#error "Hardware platform not supported! Define LEPRA_MSVC_X86/LEPRA_MSVC_ARM/LEPRA_GCC_X86/etc to compile for that platform."

#endif	// Platform



typedef unsigned char	utchar;

#define	__WIDE(x)	L ## x
#define	_WIDE(x)	__WIDE(x)
#define	_(x)		x



}
