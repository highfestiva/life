
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// #defines the basics from compiler and system
// predefines. These #defines should be used
// throughout the system to ensure maintainability
// and portability.



#pragma once



////////////////////////////// Target //////////////////////////////
#if defined(_MSC_VER)	// Microsoft Visual C++.

#define LEPRA_MSVC
#if defined(_M_IA64) || defined(_M_X64)
#define LEPRA_MSVC_X86
#define LEPRA_MSVC_X86_64
#elif defined(_M_IX86)
#define LEPRA_MSVC_X86
#define LEPRA_MSVC_X86_32
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
#elif defined(__GNUG__)	|| defined (__GNUC__) // GCC

#define LEPRA_GCC
#if defined(__x86_64__) || defined(__ia64__)
#define LEPRA_GCC_X86
#define LEPRA_GCC_X86_64
#elif defined(__i386__)
#define LEPRA_GCC_X86
#define LEPRA_GCC_X86_32
#elif defined(__POWERPC__)
#define LEPRA_GCC_POWERPC
#elif defined(__arm__)
#define LEPRA_GCC_ARM
#else	// Unknown hardware.
#error "Hardware platform not supported!"
#endif

#if defined(__APPLE__)
#define LEPRA_MAC
#if defined(LEPRA_GCC_ARM) || defined(LEPRA_IPHONE_SIMULATOR)
#define LEPRA_IOS
#endif // iOS
#endif // Mac

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
#define LEPRA_DEBUG_CODE(x)		x
#elif !defined(NO_LOG_DEBUG_INFO)
#define LEPRA_RELEASE_CANDIDATE
#define LEPRA_BUILD_TYPE_TEXT	"Release Candidate"
#define LEPRA_INFO_PERFORMANCE
#define LEPRA_DEBUG_CODE(x)
#else // FINAL
#define LEPRA_FINAL
#define LEPRA_BUILD_TYPE_TEXT	"Final"
#define LEPRA_DEBUG_CODE(x)
#endif // _DEBUG/!NO_LOG_DEBUG_INFO/FINAL

#if defined(LEPRA_UNICODE)
#define LEPRA_STRING_TYPE_TEXT	"Unicode"
#else // LEPRA_ANSI
#define LEPRA_STRING_TYPE_TEXT	"Ansi"
#endif

#if defined(LEPRA_IOS)
#define LEPRA_TOUCH
#endif // Touch device


// We keep some graphics stuff here, since primitives are defined
// in Lepra, not in UiLepra (only drawing there).
#if defined(LEPRA_IOS)
typedef unsigned short vtx_idx_t;
#else // !iOS
typedef unsigned vtx_idx_t;
#endif // iOS/!iOS
