
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/IOBuffer.h"



namespace Lepra
{



IOBuffer::IOBuffer(size_t pMaxSize) :
	mMaxSize(pMaxSize),
	mBuffer(0),
	mBufferSize(0),
	mBufferStart(0),
	mDataSize(0),
	mBufferOwner(true)
{
}

IOBuffer::IOBuffer(uint8* pBuffer, size_t pSize, size_t pDataSize) :
	mMaxSize(pSize),
	mBuffer(pBuffer),
	mBufferSize(pSize),
	mBufferStart(0),
	mDataSize(pDataSize),
	mBufferOwner(false)
{
}

IOBuffer::~IOBuffer()
{
	if (mBufferOwner == true && mBuffer != 0)
	{
		delete[] mBuffer;
	}
}

void IOBuffer::Close()
{
	// Do nothing...
}

void IOBuffer::Flush()
{
	mDataSize = 0;
}

IOError IOBuffer::WriteRaw(const void* pData, size_t pLength)
{
	if (mBufferOwner == true && (mDataSize + pLength) > mBufferSize)
	{
		ExtendDataBuffer(mDataSize + pLength);
	}

	IOError lErr = IO_OK;

	if ((mDataSize + pLength) > mBufferSize)
	{
		pLength = mBufferSize - mDataSize;
		lErr = IO_BUFFER_OVERFLOW;
	}

	// Calculate the starting position for writing (mBufferStart is for reading).
	size_t lBufferStart = (mBufferStart + mDataSize) % mBufferSize;

	if (lBufferStart + pLength > mBufferSize)
	{
		size_t lSize = mBufferSize - lBufferStart;
		memcpy(&mBuffer[lBufferStart],
			   pData,
			   (size_t)lSize);
		memcpy(mBuffer,
			   &((uint8*)pData)[lSize],
			   (size_t)(pLength - lSize));
	}
	else
	{
		memcpy(&mBuffer[lBufferStart], pData, (size_t)pLength);
	}

	mDataSize += pLength;

	return lErr;
}

IOError IOBuffer::ReadRaw(void* pData, size_t pLength)
{
	IOError lErr = IO_OK;

	if (pLength > mDataSize)
	{
		pLength = mDataSize;
		lErr = IO_BUFFER_UNDERFLOW;
	}

	if (mBufferStart + pLength > mBufferSize)
	{
		size_t lSize = mBufferSize - mBufferStart;
		memcpy(pData,
			   &mBuffer[mBufferStart],
			   (size_t)lSize);
		memcpy(&((uint8*)pData)[lSize],
			   mBuffer,
			   (size_t)(pLength - lSize));
	}
	else
	{
		memcpy(pData, &mBuffer[mBufferStart], (size_t)pLength);
	}

	mBufferStart = (mBufferStart + pLength) % mBufferSize;
	mDataSize -= pLength;

	return lErr;
}

int64 IOBuffer::GetAvailable() const
{
	return (int64)mDataSize;
}

IOError IOBuffer::Skip(size_t pLength)
{
	if (pLength > mDataSize)
	{
		pLength = mDataSize;
	}
	
	mBufferStart = (mBufferStart + pLength) % mBufferSize;
	mDataSize -= pLength;

	return IO_OK;
}

void IOBuffer::ExtendDataBuffer(size_t pMinSize)
{
	if (mMaxSize != 0 && mBufferSize >= mMaxSize)
	{
		return;
	}

	size_t lNewSize;

	if (mBufferSize <= 1)
	{
		lNewSize = pMinSize;
	}
	else
	{
		// Increase buffer size 1.5 times.
		lNewSize = (mBufferSize * 3) / 2;

		// If still less than the minimum size...
		if (lNewSize < pMinSize)
		{
			lNewSize = pMinSize;
		}
	}

	if (mMaxSize != 0 && lNewSize > mMaxSize)
	{
		lNewSize = mMaxSize;
	}

	uint8* lBuffer = new uint8[lNewSize];

	if (mBuffer != 0)
	{
		if (mBufferStart + mDataSize > mBufferSize)
		{
			size_t lSize = mBufferSize - mBufferStart;
			memcpy(lBuffer, &mBuffer[mBufferStart], lSize);
			memcpy(&lBuffer[lSize], mBuffer, mDataSize - lSize);
		}
		else
		{
			memcpy(lBuffer, &mBuffer[mBufferStart], mDataSize);
		}
		delete[] mBuffer;
	}

	mBuffer = lBuffer;
	mBufferSize = lNewSize;
	mBufferStart = 0;
}

}
