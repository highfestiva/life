#ifndef CONFIG_H
#define CONFIG_H



#define AL_BUILD_LIBRARY
#define ALUT_BUILD_LIBRARY
#define ALSOFT_VERSION		"1.10.622"



#if defined(WIN32) || defined(_WIN32)

#define HAVE_DSOUND		1
#define HAVE_WINMM		1
#define HAVE_SQRTF		1
#define HAVE_ACOSF		1
#define HAVE_ATANF		1
#define HAVE_FABSF		1
#define HAVE___INT64		1
#define HAVE_FLOAT_H		1
#define HAVE__CONTROLFP		1
#define HAVE_BASETSD_H		1
#define HAVE_SLEEP		1
#define HAVE_STAT		1
#define HAVE_TIME_H		1
#define HAVE_WINDOWS_H		1
#define _CRT_SECURE_NO_WARNINGS
#define snprintf		sprintf_s
#define strncasecmp		strnicmp
#define strcasecmp		stricmp
#define isnan			_isnan

#else	// Assume Posixy stuff... Currently compiles on my Ubuntu machine. :)

//#define HAVE_ALSA		1
#define HAVE_OSS		1
//#define HAVE_SOLARIS		1
//#define HAVE_PORTAUDIO		1
//#define HAVE_PULSEAUDIO		1
#define HAVE_DLFCN_H		1
#define HAVE_SQRTF		1
#define HAVE_ACOSF		1
#define HAVE_ATANF		1
#define HAVE_FABSF		1
#define HAVE_STRTOF		1
#define HAVE_STDINT_H		1
#define HAVE_GCC_DESTRUCTOR	1
//#define HAVE_PTHREAD_NP_H	1
#define HAVE_FLOAT_H		1
#define HAVE_FENV_H		1
#define HAVE_FESETROUND		1
#define HAVE__CONTROLFP		1

#endif // WIN32 / GCC



#if defined(__x86_64__) || defined(__ia64__)	// Modern Linux/Unix uses LP64.
#define SIZEOF_LONG		8
#define SIZEOF_UINT		8
#define SIZEOF_VOIDP		8
#elif defined(_M_IA64) || defined(_M_X64)	// Microsoft runs it's own race, as usual, with LLP64.
#define SIZEOF_LONG		4
#define SIZEOF_UINT		4
#define SIZEOF_VOIDP		8
#else // Assume LP32.
#define SIZEOF_LONG		4
#define SIZEOF_UINT		4
#define SIZEOF_VOIDP		4
#endif // Platform check.
#define SIZEOF_LONG_LONG	8



#endif // !CONFIG_H
