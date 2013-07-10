
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Simliar to a RAM disk file, but on its own buffer.



#pragma once



#include <stdio.h>
#include <stdlib.h>

#include "LepraTypes.h"
#include "File.h"
#include "InputStream.h"
#include "OutputStream.h"
#include "Thread.h"



namespace Lepra
{



class MemFile: public File, public InputStream, public OutputStream
{
	typedef File Parent;
public:
	MemFile();
	MemFile(Reader* pReader);
	MemFile(Writer* pWriter);
	MemFile(Reader* pReader, Writer* pWriter);
	virtual ~MemFile();

	// Use this to change the endian in the middle of a file read/write.
	void SetEndian(Endian::EndianType pEndian);
	Endian::EndianType GetEndian();

	void Clear();
	int64 GetSize() const;
	void Resize(size_t pSize);
	void CropHead(size_t pFinalSize);
	void* GetBuffer(size_t pMinimumSize);	// Use with caution, not thread safe.

	// Overrided from InputStream.
	int64 GetAvailable() const;
	virtual IOError ReadRaw(void* pBuffer, size_t pSize);
	IOError Skip(size_t pSize);

	// Overrided from OutputStream.
	virtual IOError WriteRaw(const void* pBuffer, size_t pSize);
	void Flush();

	void Close();

	// Overrided from Reader/Writer.
	IOError ReadData(void* pBuffer, size_t pSize);
	IOError WriteData(const void* pBuffer, size_t pSize);

	int64 Tell() const;
	int64 Seek(int64 pOffset, FileOrigin pFrom);

	const void* GetBuffer() const;
	void* GetBuffer();

private:
	Endian::EndianType mFileEndian;

	Lock mLock;
	uint8* mBuffer;
	size_t mSize;
	size_t mBufferSize;
	size_t mCurrentPos;

	// Usually just NULL. Can be set by the user to redirect the IO
	// through another reader/writer.
	Writer* mWriter;
	Reader* mReader;
};



}
