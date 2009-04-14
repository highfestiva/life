
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games

// #defines the basics from compiler and system
// predefines. These #defines should be used
// throughout the system to ensure maintainability
// and portability.



// Don't replace this one with #pragma once!
#ifndef LEPRATARGET_H
#define LEPRATARGET_H



////////////////////////////// Target //////////////////////////////
#if defined(_MSC_VER)	// Microsoft Visual C++.

#define LEPRA_MSVC
#if defined(_M_IX86)
#define LEPRA_MSVC_X86
#elif defined(_ARM) || defined(ARM)
#define LEPRA_MSVC_ARM
#else	// Unknown hardware.
#error "Hardware platform not supported!"
#endif // _X86_/ARM

#if defined(DEBUG) || defined(_DEBUG)
#define LEPRA_DEBUG
#endif // DEBUG

#if defined(UNICODE) || defined(_UNICODE)
#define LEPRA_UNICODE
#else // <ANSI>
#define LEPRA_ANSI
#endif // UNICODE/<ANSI>

#if defined(CONSOLE) || defined(_CONSOLE)
#define LEPRA_CONSOLE
#endif // UNICODE

#if defined(WIN32) || defined(_WIN32)
#define LEPRA_WINDOWS
#endif // _WINDOWS
#if defined(_WIN32_WCE)
#define LEPRA_WINDOWS_CE
#if !defined(LEPRA_WINDOWS)
#define LEPRA_WINDOWS
#endif // !LEPRA_WINDOWS
#endif // !_WIN32_WCE

////////////////////////////// Target //////////////////////////////
#elif defined(__GNUG__)	// GCC (C++).

#define LEPRA_GCC
#if defined(__i386__)
#define LEPRA_GCC_X86
#else	// Unknown hardware.
#error "Hardware platform not supported!"
#endif // _X86_/ARM

#if defined(DEBUG) || defined(_DEBUG)
#define LEPRA_DEBUG
#endif // DEBUG

#if defined(UNICODE) || defined(_UNICODE)
#define LEPRA_UNICODE
#else // <ANSI>
#define LEPRA_ANSI
#endif // UNICODE/<ANSI>

#define LEPRA_CONSOLE

#define LEPRA_POSIX

////////////////////////////// Target //////////////////////////////
#else	// Unknown compiler.

#error "Compiler/OS not supported!"

#endif	// _MSC_VER/__GNUG__



#if defined(LEPRA_DEBUG)
#define LEPRA_BUILD_TYPE_TEXT	"Debug"
#define LEPRA_INFO_PERFORMANCE
#elif !defined(NO_LOG_DEBUG_INFO)
#define LEPRA_RELEASE_CANDIDATE
#define LEPRA_BUILD_TYPE_TEXT	"Release Candidate"
#define LEPRA_INFO_PERFORMANCE
#else // FINAL
#define LEPRA_FINAL
#define LEPRA_BUILD_TYPE_TEXT	"Final"
#endif // _DEBUG/!NO_LOG_DEBUG_INFO/FINAL

#if defined(LEPRA_UNICODE)
#define LEPRA_STRING_TYPE_TEXT	"Unicode"
#else // LEPRA_ANSI
#define LEPRA_STRING_TYPE_TEXT	"Ansi"
#endif


#endif // !LEPRATARGET_H


// TRICKY: platform header files must cope with being included several times.
#if defined(LEPRA_WINDOWS)
#include "Win32/Win32Target.h"
#elif defined(LEPRA_POSIX)
#include "Posix/PosixTarget.h"
#else
#error "No recognized target platform specified!"
#endif // LEPRA_WINDOWS/LEPRA_POSIX/?
