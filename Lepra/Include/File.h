
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "Reader.h"
#include "Writer.h"



namespace Lepra
{



class File: public Reader, public Writer
{
public:
	enum FileOrigin
	{
		FSEEK_SET = 0,
		FSEEK_CUR,
		FSEEK_END,
	};
	enum //Mode flags.
	{
		READ_MODE = 1,
		WRITE_MODE = 2,
	};

	File(Endian::EndianType pReaderEndian = Endian::TYPE_BIG_ENDIAN,
		    Endian::EndianType pWriterEndian = Endian::TYPE_BIG_ENDIAN,
		    InputStream* pIn = 0,
		    OutputStream* pOut = 0);

	virtual ~File();

	virtual void Close();

	virtual void SetEndian(Endian::EndianType pEndian);

	virtual int64 GetSize() const = 0;

	IOError Skip(size_t pSize);

	// Tell and Seek both return the current file position.
	virtual int64 Tell() const = 0;
	virtual int64 Seek(int64 pOffset, FileOrigin pFrom) = 0;
	int64 SeekSet(int64 pOffset);
	int64 SeekCur(int64 pOffset);
	int64 SeekEnd(int64 pOffset);

	bool IsInMode(unsigned pMode) const;

	virtual void Flush() = 0;

	bool HasSameContent(File& pOtherFile, int64 pLength);	// Compares content by reading the both files byte by byte.

protected:
	void SetMode(unsigned pMode);
	void ClearMode(unsigned pMode);

private:
	unsigned mModeFlags;
};



}
