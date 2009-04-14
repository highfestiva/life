/*
	File:	PosixTarget.h
	Author:	Jonas Bystr�
	Copyright (c) 2002-2006, Righteous Games

	This file now only includes the system-specific header
	files and fiddles with the inherent #defines.
*/



#ifndef POSIXTARGET_H
#define POSIXTARGET_H



#ifndef LEPRA_INCLUDE_NO_OS

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <time.h>

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

#define WIDE_SPRINTF(buf, size, fmt, arg)	::vswprintf(buf, size, fmt, arg)
#define WIDE_ATOI(str)				{wchar_t* lTemp = 0; (int)::wcstol(str, &lTemp, 10);}

#define SOCKET_LAST_ERROR()			errno

namespace Lepra
{
	// OS types.
	typedef	int sys_socket;
}

#else // LEPRA_INCLUDE_NO_OS

#endif // !LEPRA_INCLUDE_NO_OS/LEPRA_INCLUDE_NO_OS

typedef int SOCKET;

#endif // !POSIXTARGET_H
