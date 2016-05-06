/*
	Class:  InputStream
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This is an interface to all classes representing an input stream of
	bytes.
*/

#pragma once

#include "lepratypes.h"
#include "string.h"
#include "ioerror.h"
#include "string.h"

namespace lepra {

class InputStream {
public:

	inline InputStream();
	inline virtual ~InputStream();

	// Closes the stream and releases any system resources associated with
	// this stream.
	virtual void Close() = 0;

	// Returns the number of available bytes in the stream.
	// This value may change over time.
	virtual int64 GetAvailable() const = 0;

	// Reads length bytes into the specified byte array from this input
	// stream.
	virtual IOError ReadRaw(void* data, size_t length) = 0;

	// Skips over and discards length bytes of data from this input stream.
	virtual IOError Skip(size_t length) = 0;

	inline void SetName(const str& name);
	inline const str& GetName();

protected:
private:
	str name_;
};

InputStream::InputStream() {
}

InputStream::~InputStream() {
}

void InputStream::SetName(const str& name) {
	name_ = name;
}

const str& InputStream::GetName() {
	return name_;
}

}
