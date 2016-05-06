
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratypes.h"
#include "endian.h"
#include "string.h"
#include "ioerror.h"

namespace lepra {



class InputStream;



class Reader {
public:

	friend class DiskFile;
	friend class ArchiveFile;
	friend class MemFile;

	Reader();
	Reader(Endian::EndianType endian);
	Reader(InputStream* in);
	Reader(InputStream* in, Endian::EndianType endian);
	virtual ~Reader();

	IOError Read(char& data);
	IOError Read(wchar_t& data);
	IOError Read(int8& data);
	IOError Read(uint8& data);
	IOError Read(int16& data);
	IOError Read(uint16& data);
	IOError Read(int32& data);
	IOError Read(uint32& data);
	IOError Read(int64& data);
	IOError Read(uint64& data);
	IOError Read(float32& data);
	IOError Read(float64& data);
	virtual IOError ReadData(void* buffer, size_t size);
	// Allocates (new[]) memory before read. If read failes, the memory is always freed before return.
	IOError AllocReadData(void** buffer, size_t size);

	// Use this to ignore data.
	virtual IOError Skip(size_t size);

	void SetReaderEndian(Endian::EndianType reader_endian);
	Endian::EndianType GetReaderEndian() const;

	const str& GetStreamName();

	int64 GetAvailable() const;

	uint64 GetReadCount();

	// Read one line of text (until '\n').
	IOError ReadLine(str& s);

protected:
	void SetInputStream(InputStream* in_stream);
private:

	uint64 read_count_;
	InputStream* in_stream_;
	Endian::EndianType reader_endian_;
};



}
