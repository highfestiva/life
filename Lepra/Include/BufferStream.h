/*
	Class:  BufferInputStream,
			BufferOutputStream
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/


#ifndef LEPRA_BUFFERSTREAM_H
#define LEPRA_BUFFERSTREAM_H


#include "InputStream.h"
#include "OutputStream.h"

namespace Lepra
{

class BufferInputStream : public InputStream
{
public:
	BufferInputStream(uint8* pBuffer, size_t pBufferSize);
	virtual ~BufferInputStream();

	virtual void Close();
	virtual int64 GetAvailable() const;
	virtual IOError ReadRaw(void* pData, size_t pLength);
	virtual IOError Skip(size_t pLength);
protected:
private:
	uint8* mBuffer;
	size_t mBufferSize;
	size_t mReadIndex;
};

class BufferOutputStream : public OutputStream
{
public:
	BufferOutputStream(uint8* pBuffer, size_t pBufferSize);
	virtual ~BufferOutputStream();

	virtual void Close();
	virtual void Flush();
	virtual IOError WriteRaw(void* pData, size_t pLength);
protected:
private:
	uint8* mBuffer;
	size_t mBufferSize;
	size_t mWriteIndex;
};

} // End namespace.

#endif // !LEPRA_BUFFERSTREAM_H
