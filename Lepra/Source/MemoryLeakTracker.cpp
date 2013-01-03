
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../Include/MemoryLeakTracker.h"

//#define MEMOVERWRITE_DETECT	1



#if defined(_DEBUG) && defined(LEPRA_WINDOWS) && defined(MEMLEAK_DETECT)

// Remove the definition...
#undef new

#pragma warning(disable: 4996)	// Deprecated functions are not to be used, as they may be removed in future versions. Circumvent problem for example in strings by using str instead.

#include <new>
#include <exception>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <crtdbg.h>
#include "../Include/SpinLock.h"



namespace Lepra
{

class MemoryLeakTracker
{
public:

	friend class MemoryLeakDetector;

	static void AddTrack(void* pAddr, unsigned long pSize, const char* pFName, unsigned long pNum);
	static bool RemoveTrack(void* pAddr);
	static void DumpLeaks();

	static int smMaxAllocated;
	static int smCurrentlyAllocated;
	static SpinLock smSpinLock;
	static bool lLeaksDumped;
};

}



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
	void*		mAddress;
	unsigned long	mSize;
	char		mFile[256];
	unsigned long	mLine;
	ALLOC_INFO*	mNextAllocInfo;
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

	smSpinLock.UncheckedAcquire();

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

	smSpinLock.UncheckedAcquire();

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

	/*if (lTrackFound == false)
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
		OutputDebugStringA(lString);
	}*/

	smSpinLock.Release();

	return lTrackFound;
};

void MemoryLeakTracker::DumpLeaks()
{
	smSpinLock.UncheckedAcquire();

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

		OutputDebugStringA(lBuf);
		lTotalSize += lCurrentInfo->mSize;

		ALLOC_INFO* lInfo = lCurrentInfo;
		lCurrentInfo = lCurrentInfo->mNextAllocInfo;
		free(lInfo);

		lNumUnfreed++;
	}

	sprintf(lBuf, "-----------------------------------------------------------\n");
	OutputDebugStringA(lBuf);
	sprintf(lBuf, "Total Unfreed: %d bytes in %d allocations.\n", lTotalSize, lNumUnfreed);
	OutputDebugStringA(lBuf);
	sprintf(lBuf, "Peak alloc: %d bytes.\n", smMaxAllocated);
	OutputDebugStringA(lBuf);

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
#include <assert.h>

void* operator new(size_t pSize)
{
#if !defined(MEMOVERWRITE_DETECT)	// Normal operation, no overwrite detection
	void* lPointer = malloc(pSize);
	if (lPointer == 0) throw std::bad_alloc(); // ANSI/ISO compliant behavior
	return lPointer;
#else // Overwrite detection - YEAH!
	char* lPointer = (char*)malloc(pSize+12);
	if (lPointer == 0) throw std::bad_alloc(); // ANSI/ISO compliant behavior
	*(int*)lPointer = 0x5AF00FA5;
	*(int*)(lPointer+4) = (int)pSize;
	*(int*)(lPointer+pSize+8) = 0x55C3F0A9;
	return lPointer+8;
#endif // Normal alloc / With overwrite detection.
}

void operator delete(void* pPointer)
{
#if !defined(MEMOVERWRITE_DETECT)	// Normal operation, no overwrite detection
	free(pPointer);
#else // Overwrite detection - YEAH!
	if (!pPointer) return;
	char* lPointer = (char*)pPointer - 8;
	assert(*(int*)lPointer == 0x5AF00FA5);
	int lSize = *(int*)(lPointer+4);
	assert(*(int*)(lPointer+lSize+8) == 0x55C3F0A9);
	free(lPointer);
#endif // Normal alloc / With overwrite detection.
}

void operator delete[](void* pPointer)
{
#if !defined(MEMOVERWRITE_DETECT)	// Normal operation, no overwrite detection
	free(pPointer);
#else // Overwrite detection - YEAH!
	if (!pPointer) return;
	char* lPointer = (char*)pPointer - 8;
	assert(*(int*)lPointer == 0x5AF00FA5);
	int lSize = *(int*)(lPointer+4);
	assert(*(int*)(lPointer+lSize+8) == 0x55C3F0A9);
	free(lPointer);
#endif // Normal alloc / With overwrite detection.
}

#endif // <Memleak detection>/!<Memleak detection>
