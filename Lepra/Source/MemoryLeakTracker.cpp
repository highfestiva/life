/*
	Class:  Memory
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	Visual C++ fix + memory leak detection in debug mode.
*/

#include "../Include/MemoryLeakTracker.h"

#if defined(_DEBUG) && defined(LEPRA_WINDOWS) && defined(MEMLEAK_DETECT)

// Remove the definition...
#undef new

#pragma warning(disable: 4996)	// Deprecated functions are not to be used, as they may be removed in future versions. Circumvent problem for example in strings by using Lepra::String instead.

#include <new>
#include <exception>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <windows.h>

void* operator new(size_t pSize, const char* pFileName, int pLine)
{
	void* lPointer = (void*)malloc(pSize);
	if (lPointer == 0)
		throw std::bad_alloc(); // ANSI/ISO compliant behavior

	Lepra::MemoryLeakTracker::AddTrack(lPointer, (unsigned long)pSize, pFileName, pLine);

	return lPointer;
}

/*void* operator new(size_t pSize)
{
	void* lPointer = (void*)malloc(pSize);
	if (lPointer == 0)
		throw std::bad_alloc(); // ANSI/ISO compliant behavior

	Lepra::MemoryLeakTracker::AddTrack(lPointer, pSize, "<Unknown file - make sure to include Lepra.h>", 0);

	return lPointer;
}
*/
void operator delete(void* pPointer, const char*, int)
{
	//_ASSERT(pPointer != 0);
	if (pPointer != 0)
	{
		Lepra::MemoryLeakTracker::RemoveTrack(pPointer);
		free(pPointer);
	}
}

void operator delete(void* pPointer)
{
	//_ASSERT(pPointer != 0);
	if (pPointer != 0)
	{
		Lepra::MemoryLeakTracker::RemoveTrack(pPointer);
		free(pPointer);
	}
}

void operator delete[](void* pPointer)
{
	//_ASSERT(pPointer != 0);
	if (pPointer != 0)
	{
		Lepra::MemoryLeakTracker::RemoveTrack(pPointer);
		free(pPointer);
	}
}

namespace Lepra
{

struct ALLOC_INFO
{
	void*				mAddress;
	unsigned long	    mSize;
	char	            mFile[256];
	unsigned long	    mLine;

	ALLOC_INFO* mNextAllocInfo;
};

ALLOC_INFO* gFirstInfo = 0;
ALLOC_INFO* gLastInfo = 0;
int gNumFalseLeaks = 0;

int MemoryLeakTracker::smMaxAllocated = 0;
int MemoryLeakTracker::smCurrentlyAllocated = 0;
SpinLock MemoryLeakTracker::smSpinLock;
bool MemoryLeakTracker::lLeaksDumped = false;

void MemoryLeakTracker::AddTrack(void* pAddr, unsigned long pSize, const char* pFName, unsigned long pNum)
{
	// TODO: optimize HARD!!!

	smSpinLock.Acquire();

	ALLOC_INFO* lInfo;

	lInfo = (ALLOC_INFO*)malloc(sizeof(ALLOC_INFO));
	lInfo->mAddress = pAddr;
	strncpy(lInfo->mFile, pFName, 255);
	lInfo->mLine = pNum;
	lInfo->mSize = pSize;
	lInfo->mNextAllocInfo = 0;

	if (gFirstInfo == 0)
	{
		gFirstInfo = lInfo;
		gLastInfo  = lInfo;
	}
	else
	{
		gLastInfo->mNextAllocInfo = lInfo;
		gLastInfo = lInfo;
	}

	smCurrentlyAllocated += pSize;
	if (smCurrentlyAllocated > smMaxAllocated)
		smMaxAllocated = smCurrentlyAllocated;

	smSpinLock.Release();
};

bool MemoryLeakTracker::RemoveTrack(void* pAddr)
{
	// TODO: optimize HARD!!!

	smSpinLock.Acquire();

	ALLOC_INFO* lCurrentInfo = gFirstInfo;
	ALLOC_INFO* lPrevInfo = 0;

	bool lTrackFound = false;

	while (lCurrentInfo != 0)
	{
		if (lCurrentInfo->mAddress == pAddr)
		{
			smCurrentlyAllocated -= lCurrentInfo->mSize;

			if (gFirstInfo == lCurrentInfo)
				gFirstInfo = lCurrentInfo->mNextAllocInfo;
			if (gLastInfo == lCurrentInfo)
				gLastInfo = lPrevInfo;

			if (lPrevInfo != 0)
			{
				lPrevInfo->mNextAllocInfo = lCurrentInfo->mNextAllocInfo;
			}

			free(lCurrentInfo);

			lTrackFound = true;
			break;
		}

		lPrevInfo = lCurrentInfo;
		lCurrentInfo = lCurrentInfo->mNextAllocInfo;
	}

	if (lTrackFound == false)
	{
		char lString[256];

		if (lLeaksDumped == true)
		{
			gNumFalseLeaks++;
			sprintf(lString, "False memory leak reported at address 0x%p, (Count: %d)\n", pAddr, gNumFalseLeaks);
		}
		else
		{
			sprintf(lString, "Released unknown memory address 0x%p (allocated in BSS = global object)\n", pAddr);
		}
		OutputDebugString(lString);
	}

	smSpinLock.Release();

	return lTrackFound;
};

void MemoryLeakTracker::DumpLeaks()
{
	smSpinLock.Acquire();

	unsigned long lTotalSize = 0;
	char lBuf[1024];

	ALLOC_INFO* lCurrentInfo = gFirstInfo;

	// Get the maximum file name length in order to pad the output
	// nice.
	int lMaxLength = 0;
	int lMaxLength2 = 0;
	int lMaxLineDigits = 0;
	while (lCurrentInfo != 0)
	{
		sprintf(lBuf, "%s()", lCurrentInfo->mFile);
		int lLength = (int)::strlen(lBuf);

		sprintf(lBuf, "%d", lCurrentInfo->mLine);
		int lLineDigits = (int)::strlen(lBuf);

		lLength += lLineDigits;

		if (lLength > lMaxLength)
			lMaxLength = lLength;
		if (lLineDigits > lMaxLineDigits)
			lMaxLineDigits = lLineDigits;

		lCurrentInfo = lCurrentInfo->mNextAllocInfo;
	}
	lMaxLength += 4;

	// 22 is the length of "ADDRESS 0x00000000    ".
	lMaxLength2 = lMaxLength + 22 + lMaxLineDigits + 1;

	lCurrentInfo = gFirstInfo;
	gFirstInfo = 0;
	gLastInfo = 0;

	int lNumUnfreed = 0;

	while (lCurrentInfo != 0)
	{
		sprintf(lBuf, "%s(%d)", lCurrentInfo->mFile, lCurrentInfo->mLine);
		int lLength = (int)::strlen(lBuf);

		// Pad with whitespace.
		int i;
		for (i = lLength; i < lMaxLength; i++)
		{
			lBuf[i] = ' ';
		}
		lBuf[lMaxLength] = 0;

		sprintf(&lBuf[lMaxLength], "ADDRESS 0x%p    %d", lCurrentInfo->mAddress,
			lCurrentInfo->mSize);

		lLength = (int)::strlen(lBuf);

		// Pad with more whitespace.
		for (i = lLength; i < lMaxLength2; i++)
		{
			lBuf[i] = ' ';
		}
		lBuf[lMaxLength2] = 0;

		sprintf(&lBuf[lMaxLength2], "bytes unfreed\n");

		OutputDebugString(lBuf);
		lTotalSize += lCurrentInfo->mSize;

		ALLOC_INFO* lInfo = lCurrentInfo;
		lCurrentInfo = lCurrentInfo->mNextAllocInfo;
		free(lInfo);

		lNumUnfreed++;
	}

	sprintf(lBuf, "-----------------------------------------------------------\n");
	OutputDebugString(lBuf);
	sprintf(lBuf, "Total Unfreed: %d bytes in %d allocations.\n", lTotalSize, lNumUnfreed);
	OutputDebugString(lBuf);
	sprintf(lBuf, "Peak alloc: %d bytes.\n", smMaxAllocated);
	OutputDebugString(lBuf);

	lLeaksDumped = true;

	smSpinLock.Release();
};

/*
void MemoryLeakTracker::InitMemoryLeakDetection()
{
	// Clear all records.
	ALLOC_INFO* lCurrentInfo = gFirstInfo;

	gFirstInfo = 0;
	gLastInfo = 0;

	while (lCurrentInfo != 0)
	{
		ALLOC_INFO* lInfo = lCurrentInfo;
		lCurrentInfo = lCurrentInfo->mNextAllocInfo;
		free(lInfo);
	}
}
*/

// TODO: place this in a systemcall so that we're the last function to
// execute (not as in this case, when we're just another global object
// being destroyed.
class MemoryLeakDetector
{
public:
	MemoryLeakDetector(){}

	~MemoryLeakDetector()
	{
		MemoryLeakTracker::DumpLeaks();
	}
};

MemoryLeakDetector gLeakDetector;

} // End namespace.

#else // !<Memleak detection>

#include <new>
#include <exception>
#include <cstdlib>

void* operator new(size_t pSize)
{
	void* lPointer = malloc(pSize);
	if (lPointer == 0)
		throw std::bad_alloc(); // ANSI/ISO compliant behavior

	return lPointer;
}

void operator delete(void* pPointer)
{
	free(pPointer);
}

void operator delete[](void* pPointer)
{
	free(pPointer);
}

#endif // <Memleak detection>/!<Memleak detection>
