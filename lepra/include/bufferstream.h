/*
	Class:  BufferInputStream,
			BufferOutputStream
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/


#pragma once

#include "inputstream.h"
#include "outputstream.h"

namespace lepra {

class BufferInputStream : public InputStream {
public:
	BufferInputStream(uint8* buffer, size_t buffer_size);
	virtual ~BufferInputStream();

	virtual void Close();
	virtual int64 GetAvailable() const;
	virtual IOError ReadRaw(void* data, size_t length);
	virtual IOError Skip(size_t length);
protected:
private:
	uint8* buffer_;
	size_t buffer_size_;
	size_t read_index_;
};

class BufferOutputStream : public OutputStream {
public:
	BufferOutputStream(uint8* buffer, size_t buffer_size);
	virtual ~BufferOutputStream();

	virtual void Close();
	virtual void Flush();
	virtual IOError WriteRaw(void* data, size_t length);
protected:
private:
	uint8* buffer_;
	size_t buffer_size_;
	size_t write_index_;
};

}
