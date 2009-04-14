
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/FdSet.h"



namespace Lepra
{



size_t FdSetHelper::Copy(FdSet& pDestination, const FdSet& pSource)
{
#ifdef LEPRA_WINDOWS
	::memcpy(&pDestination, &pSource, sizeof(pSource.fd_count)+pSource.fd_count*sizeof(pSource.fd_array[0]));
	size_t lSocketCount = pSource.fd_count;
#else // Posix
#error "Not implemented yet on Posix!"
#endif // Windows/Posix.
	return (lSocketCount);
}



}
