
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "LepraTypes.h"
#include "String.h"
#include "Endian.h"
#include "IOError.h"



namespace Lepra
{



class OutputStream;



class Writer
{
public:

	friend class DiskFile;
	friend class ArchiveFile;
	friend class MemFile;

	Writer(Endian::EndianType pEndian = Endian::TYPE_BIG_ENDIAN);
	Writer(OutputStream* pOut, Endian::EndianType pEndian = Endian::TYPE_BIG_ENDIAN);
	virtual ~Writer();

	IOError Write(const char& pData);
	IOError Write(const wchar_t& pData);
	IOError Write(const int8& pData);
	IOError Write(const uint8& pData);
	IOError Write(const int16& pData);
	IOError Write(const uint16& pData);
	IOError Write(const int32& pData);
	IOError Write(const uint32& pData);
	IOError Write(const int64& pData);
	IOError Write(const uint64& pData);
	IOError Write(const float32& pData);
	IOError Write(const float64& pData);

	virtual IOError WriteData(const void* pBuffer, size_t pSize);

	void SetWriterEndian(Endian::EndianType pWriterEndian);
	Endian::EndianType GetWriterEndian() const;
	
	const str& GetStreamName();
	
	// Writes the length of the string (excluding the implicit null-character).
	template<class _T> IOError WriteString(const std::basic_string<_T>& pString);

protected:
	void SetOutputStream(OutputStream* pOutStream);

private:

	OutputStream* mOutStream;
	Endian::EndianType mWriterEndian;
};



template<class _T>
IOError Writer::WriteString(const std::basic_string<_T>& pString)
{
	astr lUtf8 = astrutil::Encode(pString);
	return (WriteData(lUtf8.c_str(), (unsigned)lUtf8.length()));
}



}
