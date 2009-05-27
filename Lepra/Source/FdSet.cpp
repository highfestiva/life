
// Author: Jonas Bystroem
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/FdSet.h"



namespace Lepra
{



int FdSetHelper::Copy(FdSet& pDestination, const FdSet& pSource)
{
#if defined(LEPRA_WINDOWS)
	const size_t lCountAndPad = pSource.fd_array-(&pSource.fd_count);
	::memcpy(&pDestination, &pSource, lCountAndPad + pSource.fd_count*sizeof(pSource.fd_array[0]));
	return (pSource.fd_count);
#elif defined(LEPRA_POSIX)
	LEPRA_FD_ZERO(&pDestination);
	::memcpy(&pDestination.mFdSet, &pSource.mFdSet, pSource.mMaxHandle/8+8);	// Copy an int64 extra (copes big endian).
	pDestination.mSocketArray = pSource.mSocketArray;
	pDestination.mMaxHandle = pSource.mMaxHandle;
	return ((int)pSource.mSocketArray.size());
#else // Unknown
#error "Can't handle unknown fd_set struct."
#endif // Windows / Posix / other
}



}
