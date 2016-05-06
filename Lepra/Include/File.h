
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "reader.h"
#include "writer.h"



namespace lepra {



class File: public Reader, public Writer {
public:
	enum FileOrigin {
		kFseekSet = 0,
		kFseekCur,
		kFseekEnd,
	};
	enum { //Mode flags.
		kReadMode = 1,
		kWriteMode = 2,
	};

	File(Endian::EndianType reader_endian = Endian::kTypeBigEndian,
		    Endian::EndianType writer_endian = Endian::kTypeBigEndian,
		    InputStream* in = 0,
		    OutputStream* out = 0);

	virtual ~File();

	virtual void Close();

	virtual void SetEndian(Endian::EndianType endian);

	virtual int64 GetSize() const = 0;

	IOError Skip(size_t size);

	// Tell and Seek both return the current file position.
	virtual int64 Tell() const = 0;
	virtual int64 Seek(int64 offset, FileOrigin from) = 0;
	int64 SeekSet(int64 offset);
	int64 SeekCur(int64 offset);
	int64 SeekEnd(int64 offset);

	bool IsInMode(unsigned mode) const;

	virtual void Flush() = 0;

	bool HasSameContent(File& other_file, int64 length);	// Compares content by reading the both files byte by byte.

protected:
	void SetMode(unsigned mode);
	void ClearMode(unsigned mode);

private:
	unsigned mode_flags_;
};



}
