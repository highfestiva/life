
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/MemFile.h"
#include "../Include/LepraAssert.h"
#include "../Include/String.h"



namespace Lepra
{



MemFile::MemFile():
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mBuffer(0),
	mSize(0),
	mBufferSize(0),
	mCurrentPos(0),
	mWriter(0),
	mReader(0)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
}

MemFile::MemFile(Reader* pReader):
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mBuffer(0),
	mSize(0),
	mBufferSize(0),
	mCurrentPos(0),
	mWriter(0),
	mReader(pReader)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
	if (mReader != 0)
	{
		mReader->SetInputStream(this);
	}
}

MemFile::MemFile(Writer* pWriter):
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mBuffer(0),
	mSize(0),
	mBufferSize(0),
	mCurrentPos(0),
	mWriter(pWriter),
	mReader(0)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
	if (mWriter != 0)
	{
		mWriter->SetOutputStream(this);
	}
}

MemFile::MemFile(Reader* pReader, Writer* pWriter):
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mBuffer(0),
	mSize(0),
	mBufferSize(0),
	mCurrentPos(0),
	mWriter(pWriter),
	mReader(pReader)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
	if (mReader != 0)
	{
		mReader->SetInputStream(this);
	}
	if (mWriter != 0)
	{
		mWriter->SetOutputStream(this);
	}
}

MemFile::~MemFile()
{
	Close();
}

void MemFile::SetEndian(Endian::EndianType pEndian)
{
	Parent::SetEndian(pEndian);
	if (mReader != 0)
	{
		mReader->SetReaderEndian(pEndian);
	}
	if (mWriter != 0)
	{
		mWriter->SetWriterEndian(pEndian);
	}
}

int64 MemFile::Tell() const
{
	return (mCurrentPos);
}

int64 MemFile::Seek(int64 pOffset, FileOrigin pFrom)
{
	ScopeLock lLock(&mLock);
	int64 lPos = (int64)mCurrentPos;
	switch(pFrom)
	{
		case FSEEK_SET:	lPos = pOffset;			break;
		case FSEEK_CUR:	lPos += pOffset;		break;
		case FSEEK_END:	lPos = (int64)mSize + pOffset;	break;
	}
	if (lPos < 0)
	{
		mCurrentPos = 0;
	}
	else if(lPos > (int64)mSize)
	{
		mCurrentPos = mSize;
	}
	else
	{
		mCurrentPos = (size_t)lPos;
	}
	return (int64)mCurrentPos;
}

const void* MemFile::GetBuffer() const
{
	return (mBuffer);
}

void* MemFile::GetBuffer()
{
	return (mBuffer);
}

void MemFile::Clear()
{
	ScopeLock lLock(&mLock);
	mCurrentPos = 0;
	mSize = 0;
}

int64 MemFile::GetSize() const
{
	return ((int64)mSize);
}

void MemFile::Resize(size_t pSize)
{
	ScopeLock lLock(&mLock);
	if (pSize >= mBufferSize)
	{
		const size_t lNewBufferSize = pSize * 3 / 2;
		uint8* lBuffer = new uint8[lNewBufferSize];
		if (mBuffer != 0)
		{
			::memcpy(lBuffer, mBuffer, mBufferSize);
			delete[] mBuffer;
		}
		mBuffer = lBuffer;
		mBufferSize = lNewBufferSize;
	}

}

void MemFile::CropHead(size_t pFinalSize)
{
	ScopeLock lLock(&mLock);
	deb_assert(pFinalSize <= mSize);
	if (pFinalSize <= mSize)
	{
		return;
	}
	size_t lCropByteCount = mSize-pFinalSize;
	::memmove(mBuffer, mBuffer+lCropByteCount, pFinalSize);
	mSize = pFinalSize;
	if (mCurrentPos > lCropByteCount)
	{
		mCurrentPos -= lCropByteCount;
	}
	else
	{
		mCurrentPos = 0;
	}
	deb_assert(mCurrentPos <= mSize);
}

void* MemFile::GetBuffer(size_t pMinimumSize)
{
	Resize(pMinimumSize);
	mSize = pMinimumSize;
	return mBuffer;
}



int64 MemFile::GetAvailable() const
{
	return ((int64)mSize - (int64)mCurrentPos);
}

IOError MemFile::ReadRaw(void* pBuffer, size_t pSize)
{
	ScopeLock lLock(&mLock);

	if (mCurrentPos >= mSize)
	{
		return IO_BUFFER_UNDERFLOW;
	}

	size_t lEndPos = mCurrentPos + pSize;
	if (lEndPos > mSize)
	{
		::memcpy(pBuffer, &mBuffer[mCurrentPos], mSize - mCurrentPos);
		mCurrentPos = mSize;
		return IO_BUFFER_UNDERFLOW;
	}

	::memcpy(pBuffer, &mBuffer[mCurrentPos], pSize);
	mCurrentPos += pSize;
	
	return IO_OK;
}

IOError MemFile::Skip(size_t pSize)
{
	return (Parent::Skip(pSize));
}

IOError MemFile::WriteRaw(const void* pBuffer, size_t pSize)
{
	ScopeLock lLock(&mLock);

	size_t lEndPos = mCurrentPos + pSize;

	Resize(lEndPos);
	::memcpy(mBuffer+mCurrentPos, pBuffer, pSize);
	mCurrentPos += pSize;

	if (mCurrentPos > mSize)
	{
		mSize = mCurrentPos;
	}

	return (IO_OK);
}

void MemFile::Flush()
{
	// Do nothing.
}

void MemFile::Close()
{
	ScopeLock lLock(&mLock);
	delete[] mBuffer;
	mBuffer = 0;
}

IOError MemFile::ReadData(void* pBuffer, size_t pSize)
{
	ScopeLock lLock(&mLock);
	if (mReader != 0)
	{
		return mReader->ReadData(pBuffer, pSize);
	}
	else
	{
		return Reader::ReadData(pBuffer, pSize);
	}
}

IOError MemFile::WriteData(const void* pBuffer, size_t pSize)
{
	ScopeLock lLock(&mLock);
	if (mWriter != 0)
	{
		return mWriter->WriteData(pBuffer, pSize);
	}
	else
	{
		return Writer::WriteData(pBuffer, pSize);
	}
}



}
