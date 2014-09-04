
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/SubFile.h"



namespace Lepra
{



SubFile::SubFile(File* pMasterFile, int64 pFileSize) :
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN),
	mMasterFile(pMasterFile),
	mFileSize(pFileSize)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
	mFileStart = mMasterFile->Tell();
}

SubFile::~SubFile()
{
}



void SubFile::Flush()
{
}

void SubFile::Close()
{
}

int64 SubFile::GetAvailable() const
{
	int64 lOffset = Tell()-mFileStart;
	return (mFileSize-lOffset);
}

IOError SubFile::Skip(size_t pLength)
{
	return (File::Skip(pLength));
}

IOError SubFile::ReadRaw(void* pData, size_t pLength)
{
	IOError lStatus = IO_BUFFER_OVERFLOW;
	int64 lEndOffset = Tell()+pLength;
	if (mFileSize >= lEndOffset)
	{
		lStatus = mMasterFile->ReadData(pData, pLength);
	}
	return (lStatus);
}

IOError SubFile::WriteRaw(const void* pData, size_t pLength)
{
	IOError lStatus = IO_BUFFER_OVERFLOW;
	int64 lEndOffset = Tell()+pLength;
	if (mFileSize >= lEndOffset)
	{
		lStatus = mMasterFile->WriteData(pData, pLength);
	}
	return (lStatus);
}



int64 SubFile::GetSize() const
{
	return (mFileSize);
}

int64 SubFile::Tell() const
{
	int64 lOffset = mMasterFile->Tell()-mFileStart;
	return (lOffset);
}

int64 SubFile::Seek(int64 pOffset, FileOrigin pFrom)
{
	int64 lOffset = 0;
	switch(pFrom)
	{
		case FSEEK_SET:	lOffset = pOffset;		break;
		case FSEEK_CUR:	lOffset = Tell()+pOffset;	break;
		case FSEEK_END:	lOffset = mFileSize-pOffset;	break;
	};
	if (lOffset < 0)
	{
		lOffset = 0;
	}
	else if (lOffset > mFileSize)
	{
		lOffset = mFileSize;
	}
	lOffset = mMasterFile->SeekSet(lOffset+mFileStart);
	return (lOffset);
}



}
