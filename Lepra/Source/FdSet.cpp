
// Author: Jonas Bystroem
// Copyright (c) Pixel Doctrine



#include "../Include/FdSet.h"



namespace Lepra
{



int FdSetHelper::Copy(FdSet& pDestination, const FdSet& pSource)
{
#if defined(LEPRA_WINDOWS)
	const size_t lCountAndPad = ((const char*)pSource.fd_array)-((const char*)(&pSource.fd_count));
	::memcpy(&pDestination, &pSource, lCountAndPad + pSource.fd_count*sizeof(pSource.fd_array[0]));
	return (pSource.fd_count);
#elif defined(LEPRA_POSIX)
	LEPRA_FD_ZERO(&pDestination);
	::memcpy(&pDestination.mFdSet, &pSource.mFdSet, pSource.mMaxHandle/8+1);	// Copy exact amount of data (!).
	pDestination.mSocketArray = pSource.mSocketArray;
	pDestination.mMaxHandle = pSource.mMaxHandle;
	return ((int)pSource.mSocketArray.size());
#else // Unknown
#error "Can't handle unknown fd_set struct."
#endif // Windows / Posix / other
}



}
