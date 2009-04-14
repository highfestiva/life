
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/MemFile.h"
#include "../Include/String.h"



namespace Lepra
{



MemFile::MemFile():
	File(Endian::TYPE_LITTLE_ENDIAN, Endian::TYPE_LITTLE_ENDIAN, 0, 0),
	mFileEndian(Endian::TYPE_LITTLE_ENDIAN),
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
	File(Endian::TYPE_LITTLE_ENDIAN, Endian::TYPE_LITTLE_ENDIAN, 0, 0),
	mFileEndian(Endian::TYPE_LITTLE_ENDIAN),
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
	File(Endian::TYPE_LITTLE_ENDIAN, Endian::TYPE_LITTLE_ENDIAN, 0, 0),
	mFileEndian(Endian::TYPE_LITTLE_ENDIAN),
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
	File(Endian::TYPE_LITTLE_ENDIAN, Endian::TYPE_LITTLE_ENDIAN, 0, 0),
	mFileEndian(Endian::TYPE_LITTLE_ENDIAN),
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
	mFileEndian = pEndian;
	Reader::SetReaderEndian(mFileEndian);
	Writer::SetWriterEndian(mFileEndian);

	if (mReader != 0)
	{
		mReader->SetReaderEndian(mFileEndian);
	}

	if (mWriter != 0)
	{
		mWriter->SetWriterEndian(mFileEndian);
	}
}

Endian::EndianType MemFile::GetEndian()
{
	return (mFileEndian);
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
		mCurrentPos = (unsigned)lPos;
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

void MemFile::CropHead(unsigned pFinalSize)
{
	ScopeLock lLock(&mLock);
	int lCropByteCount = mSize-pFinalSize;
	if (lCropByteCount > 0)
	{
		::memcpy(mBuffer, mBuffer+lCropByteCount, pFinalSize);
		mSize = pFinalSize;
		if (mCurrentPos > (unsigned)lCropByteCount)
		{
			mCurrentPos -= lCropByteCount;
		}
		else
		{
			mCurrentPos = 0;
		}
	}
}

int64 MemFile::GetAvailable() const
{
	return ((int64)mSize - (int64)mCurrentPos);
}

IOError MemFile::ReadRaw(void* pBuffer, unsigned pSize)
{
	ScopeLock lLock(&mLock);

	if (mCurrentPos >= mSize)
	{
		return IO_BUFFER_UNDERFLOW;
	}

	unsigned lEndPos = mCurrentPos + pSize;
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

IOError MemFile::Skip(unsigned pSize)
{
	return (File::Skip(pSize));
}

IOError MemFile::WriteRaw(const void* pBuffer, unsigned pSize)
{
	ScopeLock lLock(&mLock);

	unsigned lEndPos = mCurrentPos + pSize;

	// Check if we need to allocate more memory.
	if (lEndPos > mBufferSize)
	{
		unsigned lNewBufferSize = (lEndPos * 3) / 2;
		uint8* lBuffer = new uint8[lNewBufferSize];

		if (mBuffer != 0)
		{
			::memcpy(lBuffer, mBuffer, mBufferSize);
			delete[] mBuffer;
		}

		mBuffer = lBuffer;
		mBufferSize = lNewBufferSize;
	}

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

IOError MemFile::ReadData(void* pBuffer, unsigned pSize)
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

IOError MemFile::WriteData(const void* pBuffer, unsigned pSize)
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
