/*
	Class:  IOBuffer
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	This class contains a buffer which can be accessed both as an InputStream
	as well as an OutputStream. There is intentionally no thread safety (mutex)
	implemented here.

	The buffer is treated as a cyclic array, and will grow exponentially if
	no max limit is given, and if there is more data written than read.
*/

#ifndef IOBUFFER_H
#define IOBUFFER_H

#include "LepraTypes.h"
#include "InputStream.h"
#include "OutputStream.h"

namespace Lepra
{

class IOBuffer : public InputStream, public OutputStream
{
public:
	
	IOBuffer(unsigned pMaxSize = 0);
	IOBuffer(uint8* pBuffer, unsigned pSize, unsigned pDataSize = 0);
	virtual ~IOBuffer();
	
	virtual void Close();

	virtual void Flush();
	virtual IOError WriteRaw(const void* pData, size_t pLength);
	virtual IOError ReadRaw(void* pData, size_t pLength);

	virtual int64 GetAvailable() const;
	virtual IOError Skip(size_t pLength);

protected:
private:
	void ExtendDataBuffer(unsigned pMinSize);

	unsigned mMaxSize;
	uint8* mBuffer;
	unsigned mBufferSize;
	unsigned mBufferStart;
	unsigned mDataSize;

	bool mBufferOwner;
};

} // End namespace.

#endif
