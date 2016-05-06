
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/file.h"
#include "../include/random.h"



namespace lepra {



File::File(Endian::EndianType reader_endian, Endian::EndianType writer_endian,
	InputStream* in, OutputStream* out):
	Reader(in, reader_endian),
	Writer(out, writer_endian),
	mode_flags_(0) {
}

File::~File() {
}

void File::Close() {
}

void File::SetMode(unsigned mode) {
	mode_flags_ |= mode;
}

void File::ClearMode(unsigned mode) {
	mode_flags_ &= ~mode;
}

void File::SetEndian(Endian::EndianType endian) {
	Reader::SetReaderEndian(endian);
	Writer::SetWriterEndian(endian);
}

IOError File::Skip(size_t size) {
	IOError status = kIoOk;
	int64 target_pos = Tell() + size;
	if (SeekCur(size) != target_pos) {
		status = kIoErrorReadingFromStream;
	}
	return (status);
}

int64 File::SeekSet(int64 offset) {
	return (Seek(offset, kFseekSet));
}

int64 File::SeekCur(int64 offset) {
	return (Seek(offset, kFseekCur));
}

int64 File::SeekEnd(int64 offset) {
	return (Seek(offset, kFseekEnd));
}

bool File::IsInMode(unsigned mode) const {
	return (mode_flags_ & mode) != 0;
}

bool File::HasSameContent(File& other_file, int64 length) {
	char this_char;
	char other_char;
	for (int64 x = 0; x < length; ++x) {
		if (Read(this_char) != kIoOk || other_file.Read(other_char) != kIoOk) {
			return (false);
		}
		if (this_char != other_char) {
			return (false);
		}
	}
	return ((Read(this_char) != kIoOk) == (other_file.Read(other_char) != kIoOk));
}



}
