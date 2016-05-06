/*
	Class:  OutputStream
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This is an interface to all classes representing an output stream of
	bytes.
*/

#pragma once

#include "lepratypes.h"
#include "ioerror.h"
#include "string.h"

namespace lepra {

class OutputStream {
public:

	inline OutputStream();
	inline virtual ~OutputStream();

	// Closes the stream and releases any system resources associated with
	// this stream.
	virtual void Close() = 0;

	// Flushes this output stream and forces any buffered output bytes to
	// be written out.
	virtual void Flush() = 0;

	// Writes length bytes from the specified byte array to this output
	// stream.
	virtual IOError WriteRaw(const void* data, size_t length) = 0;

	inline void SetName(const str& name);
	inline const str& GetName();

protected:
private:
	str name_;
};

OutputStream::OutputStream() {
}

OutputStream::~OutputStream() {
}

void OutputStream::SetName(const str& name) {
	name_ = name;
}

const str& OutputStream::GetName() {
	return name_;
}

}
