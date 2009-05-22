
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <vector>
#include "Lepra.h"



namespace Lepra
{



#ifdef LEPRA_WINDOWS
typedef fd_set FdSet;
#define LEPRA_FD_ZERO(fds)		FD_ZERO(fds)
#define LEPRA_FD_GET(fds, idx)		(fds)->fd_array[idx]
#define LEPRA_FD_GET_COUNT(fds)		(fds)->fd_count
#define LEPRA_FD_GET_MAX_HANDLE(fds)	FD_SETSIZE
#define LEPRA_FD_SET(s, fds)		FD_SET(s, fds)
#define LEPRA_FDS(fds)			fds
#elif defined(LEPRA_POSIX)
struct FdSet
{
	fd_set mFdSet;
	std::vector<int> mSocketArray;
};
#define LEPRA_FD_ZERO(fds)		FD_ZERO(&(fds)->mFdSet); (fds)->mSocketArray.clear()
#define LEPRA_FD_GET(fds, idx)		(fds)->mSocketArray[idx]
#define LEPRA_FD_GET_COUNT(fds)		(fds)->mSocketArray.size()
#define LEPRA_FD_GET_MAX_HANDLE(fds)	FD_SETSIZE
#define LEPRA_FD_SET(s, fds)		FD_SET(s, &(fds)->mFdSet); (fds)->mSocketArray.push_back(s)
#define LEPRA_FDS(fds)			(&(fds)->mFdSet)
#else // ???
#error fd_set and friends not defined for this platform.
#endif // Windows / Posix / ???



class FdSetHelper
{
public:
	static size_t Copy(FdSet& pDestination, const FdSet& pSource);
};



}
