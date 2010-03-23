
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/File.h"
#include "../Include/Random.h"



namespace Lepra
{



File::File(Endian::EndianType pReaderEndian, Endian::EndianType pWriterEndian,
	InputStream* pIn, OutputStream* pOut):
	Reader(pIn, pReaderEndian),
	Writer(pOut, pWriterEndian),
	mModeFlags(0)
{
}

File::~File()
{
}

void File::Close()
{
}

void File::SetMode(unsigned pMode)
{
	mModeFlags |= pMode;
}

void File::ClearMode(unsigned pMode)
{
	mModeFlags &= ~pMode;
}

bool File::GetMode(unsigned pMode)
{
	return (mModeFlags & pMode) != 0;
}

void File::SetEndian(Endian::EndianType pEndian)
{
	Reader::SetReaderEndian(pEndian);
	Writer::SetWriterEndian(pEndian);
}

IOError File::Skip(size_t pSize)
{
	IOError lStatus = IO_OK;
	int64 lTargetPos = Tell() + pSize;
	if (SeekCur(pSize) != lTargetPos)
	{
		lStatus = IO_ERROR_READING_FROM_STREAM;
	}
	return (lStatus);
}

int64 File::SeekSet(int64 pOffset)
{
	return (Seek(pOffset, FSEEK_SET));
}

int64 File::SeekCur(int64 pOffset)
{
	return (Seek(pOffset, FSEEK_CUR));
}

int64 File::SeekEnd(int64 pOffset)
{
	return (Seek(pOffset, FSEEK_END));
}

bool File::HasSameContent(File& pOtherFile, int64 pLength)
{
	char lThisChar;
	char lOtherChar;
	for (int64 x = 0; x < pLength; ++x)
	{
		if (Read(lThisChar) != IO_OK || pOtherFile.Read(lOtherChar) != IO_OK)
		{
			return (false);
		}
		if (lThisChar != lOtherChar)
		{
			return (false);
		}
	}
	return ((Read(lThisChar) != IO_OK) == (pOtherFile.Read(lOtherChar) != IO_OK));
}



}
