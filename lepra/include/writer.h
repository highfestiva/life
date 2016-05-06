
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratypes.h"
#include "string.h"
#include "endian.h"
#include "ioerror.h"



namespace lepra {



class OutputStream;



class Writer {
public:

	friend class DiskFile;
	friend class ArchiveFile;
	friend class MemFile;

	Writer(Endian::EndianType endian = Endian::kTypeBigEndian);
	Writer(OutputStream* out, Endian::EndianType endian = Endian::kTypeBigEndian);
	virtual ~Writer();

	IOError Write(const char& data);
	IOError Write(const wchar_t& data);
	IOError Write(const int8& data);
	IOError Write(const uint8& data);
	IOError Write(const int16& data);
	IOError Write(const uint16& data);
	IOError Write(const int32& data);
	IOError Write(const uint32& data);
	IOError Write(const int64& data);
	IOError Write(const uint64& data);
	IOError Write(const float32& data);
	IOError Write(const float64& data);

	virtual IOError WriteData(const void* buffer, size_t size);

	void SetWriterEndian(Endian::EndianType writer_endian);
	Endian::EndianType GetWriterEndian() const;

	const str& GetStreamName();

	// Writes the length of the string (excluding the implicit null-character).
	IOError WriteString(const str& s);

protected:
	void SetOutputStream(OutputStream* out_stream);

private:

	OutputStream* out_stream_;
	Endian::EndianType writer_endian_;
};



}
