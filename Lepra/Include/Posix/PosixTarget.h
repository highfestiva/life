
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// TRICKY: do NOT add "#pragma once" to this file.



#if !defined(LEPRA_INCLUDE_NO_OS) && !defined(LEPRA_OS_INCLUDED)
#define LEPRA_OS_INCLUDED

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <netdb.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

#define LEPRA_HEAP_CHECK()

#define WIDE_SPRINTF(buf, size, fmt, arg)	::vswprintf(buf, size, fmt, arg)
#define WIDE_ATOI(str)				{wchar_t* lTemp = 0; (int)::wcstol(str, &lTemp, 10);}

#define SOCKET_LAST_ERROR()			errno

#define LEPRA_LONGLONG(x)			x ## LL
#define LEPRA_ULONGLONG(x)			x ## ULL



#if defined(__cplusplus)
namespace Lepra
{
	// OS types.
	typedef	int sys_socket;
}
#endif // C++



#endif // !LEPRA_INCLUDE_NO_OS
