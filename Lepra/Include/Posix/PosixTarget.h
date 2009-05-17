
// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games

// TRICKY: do NOT add "#pragma once" to this file.



#if !defined(LEPRA_INCLUDE_NO_OS) && !defined(LEPRA_OS_INCLUDED)
#define LEPRA_OS_INCLUDED

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

#endif // !LEPRA_INCLUDE_NO_OS
