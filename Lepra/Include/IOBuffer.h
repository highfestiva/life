
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "LepraTypes.h"
#include "InputStream.h"
#include "OutputStream.h"



namespace Lepra
{



class IOBuffer : public InputStream, public OutputStream
{
public:	
	IOBuffer(size_t pMaxSize = 0);
	IOBuffer(uint8* pBuffer, size_t pSize, size_t pDataSize = 0);
	virtual ~IOBuffer();
	
	virtual void Close();

	virtual void Flush();
	virtual IOError WriteRaw(const void* pData, size_t pLength);
	virtual IOError ReadRaw(void* pData, size_t pLength);

	virtual int64 GetAvailable() const;
	virtual IOError Skip(size_t pLength);

protected:
private:
	void ExtendDataBuffer(size_t pMinSize);

	size_t mMaxSize;
	uint8* mBuffer;
	size_t mBufferSize;
	size_t mBufferStart;
	size_t mDataSize;

	bool mBufferOwner;
};



}
