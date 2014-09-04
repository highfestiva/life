/*
	Class:  BufferInputStream,
			BufferOutputStream
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../Include/BufferStream.h"
#include <memory.h>

namespace Lepra
{

BufferInputStream::BufferInputStream(uint8* pBuffer, size_t pBufferSize) :
	mBuffer(pBuffer),
	mBufferSize(pBufferSize),
	mReadIndex(0)
{
	if (mBuffer == 0)
		mBufferSize = 0;
}

BufferInputStream::~BufferInputStream()
{
}

void BufferInputStream::Close()
{
}

int64 BufferInputStream::GetAvailable() const
{
	return (int64)(mBufferSize - mReadIndex);
}

IOError BufferInputStream::ReadRaw(void* pData, size_t pLength)
{
	IOError lError = IO_OK;
	size_t lCopyLength = pLength;

	if ((mBufferSize - mReadIndex) < pLength)
	{
		lCopyLength = mBufferSize - mReadIndex;
		lError = IO_BUFFER_UNDERFLOW;
	}

	if (lCopyLength > 0)
	{
		memcpy(pData, &mBuffer[mReadIndex], lCopyLength);
		mReadIndex += lCopyLength;
	}

	return lError;
}

IOError BufferInputStream::Skip(size_t pLength)
{
	mReadIndex += pLength;
	if (mReadIndex > mBufferSize)
	{
		mReadIndex = mBufferSize;
		return IO_BUFFER_UNDERFLOW;
	}
	return IO_OK;
}




BufferOutputStream::BufferOutputStream(uint8* pBuffer, size_t pBufferSize) :
	mBuffer(pBuffer),
	mBufferSize(pBufferSize),
	mWriteIndex(0)
{
	if (mBuffer == 0)
		mBufferSize = 0;
}

BufferOutputStream::~BufferOutputStream()
{
}

void BufferOutputStream::Close()
{
}

void BufferOutputStream::Flush()
{
}

IOError BufferOutputStream::WriteRaw(void* pData, size_t pLength)
{
	IOError lError = IO_OK;
	size_t lCopyLength = pLength;

	if ((mBufferSize - mWriteIndex) < pLength)
	{
		lCopyLength = mBufferSize - mWriteIndex;
		lError = IO_BUFFER_OVERFLOW;
	}

	if (lCopyLength > 0)
	{
		memcpy(&mBuffer[mWriteIndex], pData, lCopyLength);
		mWriteIndex += lCopyLength;
	}

	return lError;
}

} // End namespace.
