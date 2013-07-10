/*
	Class:  OutputStream
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This is an interface to all classes representing an output stream of 
	bytes.
*/

#ifndef LEPRA_OUTPUTSTREAM_H
#define LEPRA_OUTPUTSTREAM_H

#include "LepraTypes.h"
#include "IOError.h"
#include "String.h"

namespace Lepra
{

class OutputStream
{
public:
	
	inline OutputStream();
	inline virtual ~OutputStream();
	
	// Closes the stream and releases any system resources associated with 
	// this stream.
	virtual void Close() = 0;

	// Flushes this output stream and forces any buffered output bytes to 
	// be written out.
	virtual void Flush() = 0;

	// Writes pLength bytes from the specified byte array to this output 
	// stream.
	virtual IOError WriteRaw(const void* pData, size_t pLength) = 0;

	inline void SetName(const str& pName);
	inline const str& GetName();

protected:
private:
	str mName;
};

OutputStream::OutputStream()
{
}

OutputStream::~OutputStream()
{
}

void OutputStream::SetName(const str& pName)
{
	mName = pName;
}

const str& OutputStream::GetName()
{
	return mName;
}

} // End namespace.

#endif // !LEPRA_OUTPUTSTREAM_H
