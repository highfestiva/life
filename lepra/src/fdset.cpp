
// Author: Jonas Bystroem
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/fdset.h"



namespace lepra {



int FdSetHelper::Copy(FdSet& destination, const FdSet& source) {
#if defined(LEPRA_WINDOWS)
	const size_t count_and_pad = ((const char*)source.fd_array)-((const char*)(&source.fd_count));
	::memcpy(&destination, &source, count_and_pad + source.fd_count*sizeof(source.fd_array[0]));
	return (source.fd_count);
#elif defined(LEPRA_POSIX)
	LEPRA_FD_ZERO(&destination);
	::memcpy(&destination.fd_set_, &source.fd_set_, source.max_handle_/8+1);	// Copy exact amount of data (!).
	destination.socket_array_ = source.socket_array_;
	destination.max_handle_ = source.max_handle_;
	return ((int)source.socket_array_.size());
#else // Unknown
#error "Can't handle unknown fd_set struct."
#endif // Windows / Posix / other
}



}
