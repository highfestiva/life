/*
	Class:  Writer
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#ifndef LEPRA_WRITER_H
#define LEPRA_WRITER_H

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

	inline Writer(Endian::EndianType pEndian = Endian::TYPE_LITTLE_ENDIAN) :
		mOutStream(0),
		mWriterEndian(pEndian)
	{
	}

	inline Writer(OutputStream* pOut,
				  Endian::EndianType pEndian = Endian::TYPE_LITTLE_ENDIAN) :
		mOutStream(pOut),
		mWriterEndian(pEndian)
	{
	}

	inline virtual ~Writer()
	{
	}

	inline IOError Write(const char& pData);
	inline IOError Write(const wchar_t& pData);
	inline IOError Write(const int8& pData);
	inline IOError Write(const uint8& pData);
	inline IOError Write(const int16& pData);
	inline IOError Write(const uint16& pData);
	inline IOError Write(const int32& pData);
	inline IOError Write(const uint32& pData);
	inline IOError Write(const int64& pData);
	inline IOError Write(const uint64& pData);
	inline IOError Write(const float32& pData);
	inline IOError Write(const float64& pData);

	virtual IOError WriteData(const void* pBuffer, size_t pSize);

	// Writes the string, but not the null-character. Doesn't append '\n'.
	inline IOError WriteLine(const String& pString);

	inline void SetWriterEndian(Endian::EndianType pWriterEndian);
	inline Endian::EndianType GetWriterEndian() const;
	
	inline const String& GetStreamName();
	
protected:
	inline void SetOutputStream(OutputStream* pOutStream);

private:

	OutputStream* mOutStream;
	Endian::EndianType mWriterEndian;
};

IOError Writer::Write(const char& pData)
{
	return (WriteData(&pData, sizeof(pData)));
}

IOError Writer::Write(const wchar_t& pData)
{
	return (WriteData(&pData, sizeof(pData)));
}

IOError Writer::Write(const int8& pData)
{
	return WriteData(&pData, sizeof(int8));
}

IOError Writer::Write(const uint8& pData)
{
	return WriteData(&pData, sizeof(uint8));
}

IOError Writer::Write(const int16& pData)
{
	int16 lData = Endian::HostTo(mWriterEndian, pData);
	return WriteData(&lData, sizeof(int16));
}

IOError Writer::Write(const uint16& pData)
{
	uint16 lData = Endian::HostTo(mWriterEndian, pData);
	return WriteData(&lData, sizeof(uint16));
}

IOError Writer::Write(const int32& pData)
{
	int lData = Endian::HostTo(mWriterEndian, pData);
	return WriteData(&lData, sizeof(int));
}

IOError Writer::Write(const uint32& pData)
{
	unsigned lData = Endian::HostTo(mWriterEndian, pData);
	return WriteData(&lData, sizeof(unsigned));
}

IOError Writer::Write(const int64& pData)
{
	int64 lData = Endian::HostTo(mWriterEndian, pData);
	return WriteData(&lData, sizeof(int64));
}

IOError Writer::Write(const uint64& pData)
{
	uint64 lData = Endian::HostTo(mWriterEndian, pData);
	return WriteData(&lData, sizeof(uint64));
}

IOError Writer::Write(const float32& pData)
{
	float32 lData = Endian::HostTo(mWriterEndian, pData);
	return WriteData(&lData, sizeof(float32));
}

IOError Writer::Write(const float64& pData)
{
	float64 lData = Endian::HostTo(mWriterEndian, pData);
	return WriteData(&lData, sizeof(float64));
}

IOError Writer::WriteLine(const String& pString)
{
	return (WriteData(pString.c_str(), (unsigned)pString.length()*sizeof(tchar)));
}

void Writer::SetWriterEndian(Endian::EndianType pWriterEndian)
{
	mWriterEndian = pWriterEndian;
}

Endian::EndianType Writer::GetWriterEndian() const
{
	return mWriterEndian;
}

void Writer::SetOutputStream(OutputStream* pOutStream)
{
	mOutStream = pOutStream;
}

} // End namespace.

#endif // !LEPRA_WRITER_H
