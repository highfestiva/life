
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Simliar to a RAM disk file, but on its own buffer.



#pragma once



#include <stdio.h>
#include <stdlib.h>

#include "lepratypes.h"
#include "file.h"
#include "inputstream.h"
#include "outputstream.h"
#include "thread.h"



namespace lepra {



class MemFile: public File, public InputStream, public OutputStream {
	typedef File Parent;
public:
	MemFile();
	MemFile(Reader* reader);
	MemFile(Writer* writer);
	MemFile(Reader* reader, Writer* writer);
	virtual ~MemFile();

	// Use this to change the endian in the middle of a file read/write.
	void SetEndian(Endian::EndianType endian);
	Endian::EndianType GetEndian();

	void Clear();
	int64 GetSize() const;
	void Resize(size_t size);
	void CropHead(size_t final_size);
	void* GetBuffer(size_t minimum_size);	// Use with caution, not thread safe.

	// Overrided from InputStream.
	int64 GetAvailable() const;
	virtual IOError ReadRaw(void* buffer, size_t size);
	IOError Skip(size_t size);

	// Overrided from OutputStream.
	virtual IOError WriteRaw(const void* buffer, size_t size);
	void Flush();

	void Close();

	// Overrided from Reader/Writer.
	IOError ReadData(void* buffer, size_t size);
	IOError WriteData(const void* buffer, size_t size);

	int64 Tell() const;
	int64 Seek(int64 offset, FileOrigin from);

	const void* GetBuffer() const;
	void* GetBuffer();

private:
	Lock lock_;
	uint8* buffer_;
	size_t size_;
	size_t buffer_size_;
	size_t current_pos_;

	// Usually just NULL. Can be set by the user to redirect the IO
	// through another reader/writer.
	Writer* writer_;
	Reader* reader_;
};



}
