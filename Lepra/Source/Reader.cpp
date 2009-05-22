/*
	Class:  Reader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../Include/Reader.h"
#include "../Include/InputStream.h"

namespace Lepra
{

Reader::Reader() :
	mReadCount(0),
	mInStream(0),
	mReaderEndian(Endian::TYPE_LITTLE_ENDIAN)
{
}

Reader::Reader(Lepra::Endian::EndianType pEndian) :
	mReadCount(0),
	mInStream(0),
	mReaderEndian(pEndian)
{
}

Reader::Reader(InputStream* pIn) :
	mReadCount(0),
	mInStream(pIn),
	mReaderEndian(Endian::TYPE_LITTLE_ENDIAN)
{
}

Reader::Reader(InputStream* pIn, Endian::EndianType pEndian) :
	mReadCount(0),
	mInStream(pIn),
	mReaderEndian(pEndian)
{
}

Reader::~Reader()
{
}

IOError Reader::Read(char& pData)
{
	return (ReadData(&pData, sizeof(pData)));
}

IOError Reader::Read(wchar_t& pData)
{
	return (ReadData(&pData, sizeof(pData)));
}

IOError Reader::Read(int8& pData)
{
	return ReadData(&pData, sizeof(int8));
}

IOError Reader::Read(uint8& pData)
{
	return ReadData(&pData, sizeof(uint8));
}

IOError Reader::Read(int16& pData)
{
	int16 lData;
	IOError lErr = ReadData(&lData, sizeof(int16));
	pData = Endian::HostTo(mReaderEndian, lData);

	return lErr;
}

IOError Reader::Read(uint16& pData)
{
	uint16 lData;
	IOError lErr = ReadData(&lData, sizeof(uint16));
	pData = Endian::HostTo(mReaderEndian, lData);
	return lErr;
}

IOError Reader::Read(int32& pData)
{
	int lData;
	IOError lErr = ReadData(&lData, sizeof(int));
	pData = Endian::HostTo(mReaderEndian, lData);
	return lErr;
}

IOError Reader::Read(uint32& pData)
{
	unsigned lData;
	IOError lErr = ReadData(&lData, sizeof(unsigned));
	pData = Endian::HostTo(mReaderEndian, lData);
	return lErr;
}

IOError Reader::Read(int64& pData)
{
	int64 lData;
	IOError lErr = ReadData(&lData, sizeof(int64));
	pData = Endian::HostTo(mReaderEndian, lData);
	return lErr;
}

IOError Reader::Read(uint64& pData)
{
	uint64 lData;
	IOError lErr = ReadData(&lData, sizeof(uint64));
	pData = Endian::HostTo(mReaderEndian, lData);
	return lErr;
}

IOError Reader::Read(float32& pData)
{
	float32 lData;
	IOError lErr = ReadData(&lData, sizeof(float32));
	pData = Endian::HostTo(mReaderEndian, lData);
	return lErr;
}

IOError Reader::Read(float64& pData)
{
	float64 lData;
	IOError lErr = ReadData(&lData, sizeof(float64));
	pData = Endian::HostTo(mReaderEndian, lData);
	return lErr;
}

IOError Reader::ReadData(void* pBuffer, size_t pSize)
{
	mReadCount += (uint64)pSize;
	return mInStream->ReadRaw(pBuffer, pSize);
}

IOError Reader::AllocReadData(void** pBuffer, size_t pSize)
{
	mReadCount += (uint64)pSize;
	char* lData = new char[pSize];
	IOError lStatus = ReadData(lData, pSize);
	if (lStatus == IO_OK)
	{
		*pBuffer = (void*)lData;
	}
	else
	{
		delete[] (lData);
	}
	return (lStatus);
}

IOError Reader::ReadLine(String& pString)
{
	pString.clear();

	tchar lChar;
	IOError lErr = Read(lChar);
	
	if (lErr != IO_OK)
	{
		return lErr;
	}

	while (lChar != 0 && lChar != _T('\n'))
	{
		if (lChar != '\r')
		{
			pString += lChar;
		}

		lErr = Read(lChar);
		if (lErr != IO_OK)
		{
			return lErr;
		}
	}

	return IO_OK;
}

IOError Reader::Skip(size_t pSize)
{
	return mInStream->Skip(pSize);
}

void Reader::SetReaderEndian(Endian::EndianType pReaderEndian)
{
	mReaderEndian = pReaderEndian;
}

Endian::EndianType Reader::GetReaderEndian() const
{
	return mReaderEndian;
}

const String& Reader::GetStreamName()
{
	return mInStream->GetName();
}

int64 Reader::GetAvailable() const
{
	return mInStream->GetAvailable();
}

uint64 Reader::GetReadCount()
{
	return mReadCount;
}

void Reader::SetInputStream(InputStream* pInStream)
{
	mReadCount = 0;
	mInStream = pInStream;
}

}
