
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <vector>
#include "lepraos.h"



namespace lepra {



#ifdef LEPRA_WINDOWS
typedef fd_set FdSet;
#define LEPRA_FD_ZERO(fds)		FD_ZERO(fds)
#define LEPRA_FD_GET(fds, idx)		((fds)->fd_array[idx])
#define LEPRA_FD_GET_COUNT(fds)		((fds)->fd_count)
#define LEPRA_FD_GET_MAX_HANDLE(fds)	FD_SETSIZE
#define LEPRA_FD_SET(s, fds)		FD_SET(s, fds)
#define LEPRA_FDS(fds)			fds
#elif defined(LEPRA_POSIX)
struct FdSet {
	fd_set fd_set_;
	std::vector<int> socket_array_;
	int max_handle_;
};
#define LEPRA_FD_ZERO(fds)		FD_ZERO(&(fds)->fd_set_); (fds)->socket_array_.clear(); (fds)->max_handle_ = -1
#define LEPRA_FD_GET(fds, idx)		((fds)->socket_array_[idx])
#define LEPRA_FD_GET_COUNT(fds)		((fds)->socket_array_.size())
#define LEPRA_FD_GET_MAX_HANDLE(fds)	((fds)->max_handle_)
#define LEPRA_FD_SET(s, fds)		FD_SET(s, &(fds)->fd_set_); (fds)->socket_array_.push_back(s); if ((s) > (fds)->max_handle_) (fds)->max_handle_ = (s)
#define LEPRA_FDS(fds)			(&(fds)->fd_set_)
#else // ???
#error fd_set and friends not defined for this platform.
#endif // Windows / Posix / ???



class FdSetHelper {
public:
	static int Copy(FdSet& destination, const FdSet& source);
};



}
