
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratypes.h"
#include "inputstream.h"
#include "outputstream.h"



namespace lepra {



class IOBuffer : public InputStream, public OutputStream {
public:
	IOBuffer(size_t max_size = 0);
	IOBuffer(uint8* buffer, size_t size, size_t data_size = 0);
	virtual ~IOBuffer();

	virtual void Close();

	virtual void Flush();
	virtual IOError WriteRaw(const void* data, size_t length);
	virtual IOError ReadRaw(void* data, size_t length);

	virtual int64 GetAvailable() const;
	virtual IOError Skip(size_t length);

protected:
private:
	void ExtendDataBuffer(size_t min_size);

	size_t max_size_;
	uint8* buffer_;
	size_t buffer_size_;
	size_t buffer_start_;
	size_t data_size_;

	bool buffer_owner_;
};



}
