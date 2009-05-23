
// Author: Jonas Bystroem
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/FdSet.h"



namespace Lepra
{



int FdSetHelper::Copy(FdSet& pDestination, const FdSet& pSource)
{
#ifdef LEPRA_WINDOWS
	::memcpy(&pDestination, &pSource, sizeof(pSource.fd_count)+pSource.fd_count*sizeof(pSource.fd_array[0]));
	return (pSource.fd_count);
#else // Posix
	int lSocketCount = (int)pSource.mSocketArray.size();
	if (lSocketCount)
	{
		int lMaxSocket = pSource.mSocketArray[lSocketCount-1];
		::memcpy(&pDestination.mFdSet, &pSource.mFdSet, lMaxSocket/8+1);
		pDestination.mSocketArray = pSource.mSocketArray;
	}
	else
	{
		LEPRA_FD_ZERO(&pDestination);
	}
	return (lSocketCount);
#endif // Windows / Posix.
}



}
