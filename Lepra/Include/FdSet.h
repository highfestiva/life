
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "Lepra.h"



namespace Lepra
{



#ifdef LEPRA_WINDOWS
typedef fd_set FdSet;
#define LEPRA_FD_ZERO(fds)		FD_ZERO(fds)
#define LEPRA_FD_GET(fds, idx)		(fds).fd_array[idx]
#define LEPRA_FD_GET_COUNT(fds)		(fds).fd_count
#define LEPRA_FD_GET_MAX_HANDLE(fds)	FD_SETSIZE
#define LEPRA_FD_SET(s, fds)		FD_SET(s, fds)
#define LEPRA_FDS(fds)			fds
#else // Posix
struct FdSet
{
	unsigned mSocketCount;
	sys_socket mSocketSet[FD_SETSIZE];
	fd_set mNativeFdSet;
};
#endif // Windows / Posix.



class FdSetHelper
{
public:
	static size_t Copy(FdSet& pDestination, const FdSet& pSource);
};



}
