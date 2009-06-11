/*
	Class:  File
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	This class defines the interface for a file reader/writer.

	The only difference between a file and an IO-stream is that
	a file allows random access.
*/

#ifndef FILEINTERFACE_H
#define FILEINTERFACE_H

#include "Reader.h"
#include "Writer.h"

namespace Lepra
{

class File : public Reader, public Writer
{
public:
	enum FileOrigin
	{
		FSEEK_SET = 0,
		FSEEK_CUR,
		FSEEK_END,
	};

	inline File(Endian::EndianType pReaderEndian = Endian::TYPE_LITTLE_ENDIAN,
		    Endian::EndianType pWriterEndian = Endian::TYPE_LITTLE_ENDIAN,
		    InputStream* pIn = 0,
		    OutputStream* pOut = 0);

	inline virtual ~File();

	inline virtual void Close() {};

	virtual void SetEndian(Endian::EndianType pEndian);

	virtual int64 GetSize() const = 0;

	IOError Skip(size_t pSize);

	// Tell and Seek both return the current file position.
	virtual int64 Tell() const = 0;
	virtual int64 Seek(int64 pOffset, FileOrigin pFrom) = 0;
	int64 SeekSet(int64 pOffset);
	int64 SeekCur(int64 pOffset);
	int64 SeekEnd(int64 pOffset);

	// ReadString() reads until '\0'. Returns the length of the string. -1 on error.
	int64 ReadString(String& pString);

	// Writes the entire string, including the terminating null-character.
	bool WriteStringZero(const String& pString);

	virtual void Flush() = 0;

	// Compares a filename against a string which may contain wild cards (* and ?).
	// JB: doesn't work: static bool CompareFileName(const String& pFileName, const String& pCompareString);
	// (and still has to be moved into a more sensible spot like Lepra::Path.

protected:
	enum //Mode flags.
	{
		READ_MODE = 1,
		WRITE_MODE = 2,
	};

	inline void SetMode(unsigned pMode);
	inline void ClearMode(unsigned pMode);
	inline bool GetMode(unsigned pMode);

private:
	unsigned mModeFlags;
};

File::File(Endian::EndianType pReaderEndian,
	   Endian::EndianType pWriterEndian,
	   InputStream* pIn,
	   OutputStream* pOut) :
	Reader(pIn, pReaderEndian),
	Writer(pOut, pWriterEndian),
	mModeFlags(0)
{
}

File::~File()
{
}

void File::SetMode(unsigned pMode)
{
	mModeFlags |= pMode;
}

void File::ClearMode(unsigned pMode)
{
	mModeFlags &= ~pMode;
}

bool File::GetMode(unsigned pMode)
{
	return (mModeFlags & pMode) != 0;
}

} // End namespace.

#endif
