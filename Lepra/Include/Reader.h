
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "LepraTypes.h"
#include "Endian.h"
#include "String.h"
#include "IOError.h"

namespace Lepra
{



class InputStream;



class Reader
{
public:

	friend class DiskFile;
	friend class ArchiveFile;
	friend class MemFile;

	Reader();
	Reader(Endian::EndianType pEndian);
	Reader(InputStream* pIn);
	Reader(InputStream* pIn, Endian::EndianType pEndian);
	virtual ~Reader();

	IOError Read(char& pData);
	IOError Read(wchar_t& pData);
	IOError Read(int8& pData);
	IOError Read(uint8& pData);
	IOError Read(int16& pData);
	IOError Read(uint16& pData);
	IOError Read(int32& pData);
	IOError Read(uint32& pData);
	IOError Read(int64& pData);
	IOError Read(uint64& pData);
	IOError Read(float32& pData);
	IOError Read(float64& pData);
	virtual IOError ReadData(void* pBuffer, size_t pSize);
	// Allocates (new[]) memory before read. If read failes, the memory is always freed before return.
	IOError AllocReadData(void** pBuffer, size_t pSize);

	// Use this to ignore data.
	virtual IOError Skip(size_t pSize);

	void SetReaderEndian(Endian::EndianType pReaderEndian);
	Endian::EndianType GetReaderEndian() const;

	const str& GetStreamName();

	int64 GetAvailable() const;

	uint64 GetReadCount();

	// Read one line of text (until '\n').
	template<class _T> IOError ReadLine(std::basic_string<_T>& pString);

protected:
	void SetInputStream(InputStream* pInStream);
private:

	uint64 mReadCount;
	InputStream* mInStream;
	Endian::EndianType mReaderEndian;
};



template<class _T>
IOError Reader::ReadLine(std::basic_string<_T>& pString)
{
	astr lUtf8Line;
	char lChar = '\0';
	IOError lErr = Read(lChar);
	if (lErr != IO_OK)
	{
		return (lErr);
	}
	while (lChar != '\0' && lChar != _T('\n'))
	{
		if (lChar != '\r')
		{
			lUtf8Line += lChar;
		}
		lErr = Read(lChar);
		if (lErr != IO_OK)
		{
			if (lErr == IO_ERROR_READING_FROM_STREAM)
			{
				break;	// We'll take the error on the next (=empty) line.
			}
			return (lErr);
		}
	}

	pString = StringUtilityTemplate<std::basic_string<_T> >::Encode(lUtf8Line);

	return (IO_OK);
}



}
