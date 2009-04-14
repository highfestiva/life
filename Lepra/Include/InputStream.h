/*
	Class:  InputStream
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	This is an interface to all classes representing an input stream of
	bytes.
*/

#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include "LepraTypes.h"
#include "String.h"
#include "IOError.h"
#include "String.h"

namespace Lepra
{

class InputStream
{
public:
	
	inline InputStream();
	inline virtual ~InputStream();

	// Closes the stream and releases any system resources associated with 
	// this stream.
	virtual void Close() = 0;

	// Returns the number of available bytes in the stream.
	// This value may change over time.
	virtual int64 GetAvailable() const = 0;

	// Reads pLength bytes into the specified byte array from this input 
	// stream.
	virtual IOError ReadRaw(void* pData, size_t pLength) = 0;

	// Skips over and discards pLength bytes of data from this input stream.
	virtual IOError Skip(size_t pLength) = 0;

	inline void SetName(const String& pName);
	inline const String& GetName();

protected:
private:
	String mName;
};

InputStream::InputStream()
{
}

InputStream::~InputStream()
{
}

void InputStream::SetName(const String& pName)
{
	mName = pName;
}

const String& InputStream::GetName()
{
	return mName;
}

} // End namespace.

#endif
