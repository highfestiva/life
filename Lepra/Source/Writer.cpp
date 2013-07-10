
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/Writer.h"
#include "../Include/OutputStream.h"



namespace Lepra
{



Writer::Writer(Endian::EndianType pEndian):
	mOutStream(0),
	mWriterEndian(pEndian)
{
}

Writer::Writer(OutputStream* pOut, Endian::EndianType pEndian):
	mOutStream(pOut),
	mWriterEndian(pEndian)
{
}

Writer::~Writer()
{
}

IOError Writer::WriteData(const void* pBuffer, size_t pSize)
{
	return mOutStream->WriteRaw(pBuffer, pSize);
}

const str& Writer::GetStreamName()
{
	return mOutStream->GetName();
}

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



}
