
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/memfile.h"
#include "../include/lepraassert.h"
#include "../include/string.h"



namespace lepra {



MemFile::MemFile():
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian, 0, 0),
	buffer_(0),
	size_(0),
	buffer_size_(0),
	current_pos_(0),
	writer_(0),
	reader_(0) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
}

MemFile::MemFile(Reader* reader):
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian, 0, 0),
	buffer_(0),
	size_(0),
	buffer_size_(0),
	current_pos_(0),
	writer_(0),
	reader_(reader) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
	if (reader_ != 0) {
		reader_->SetInputStream(this);
	}
}

MemFile::MemFile(Writer* writer):
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian, 0, 0),
	buffer_(0),
	size_(0),
	buffer_size_(0),
	current_pos_(0),
	writer_(writer),
	reader_(0) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
	if (writer_ != 0) {
		writer_->SetOutputStream(this);
	}
}

MemFile::MemFile(Reader* reader, Writer* writer):
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian, 0, 0),
	buffer_(0),
	size_(0),
	buffer_size_(0),
	current_pos_(0),
	writer_(writer),
	reader_(reader) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
	if (reader_ != 0) {
		reader_->SetInputStream(this);
	}
	if (writer_ != 0) {
		writer_->SetOutputStream(this);
	}
}

MemFile::~MemFile() {
	Close();
}

void MemFile::SetEndian(Endian::EndianType endian) {
	Parent::SetEndian(endian);
	if (reader_ != 0) {
		reader_->SetReaderEndian(endian);
	}
	if (writer_ != 0) {
		writer_->SetWriterEndian(endian);
	}
}

int64 MemFile::Tell() const {
	return (current_pos_);
}

int64 MemFile::Seek(int64 offset, FileOrigin from) {
	ScopeLock lock(&lock_);
	int64 pos = (int64)current_pos_;
	switch(from) {
		case kFseekSet:	pos = offset;			break;
		case kFseekCur:	pos += offset;		break;
		case kFseekEnd:	pos = (int64)size_ + offset;	break;
	}
	if (pos < 0) {
		current_pos_ = 0;
	} else if(pos > (int64)size_) {
		current_pos_ = size_;
	} else {
		current_pos_ = (size_t)pos;
	}
	return (int64)current_pos_;
}

const void* MemFile::GetBuffer() const {
	return (buffer_);
}

void* MemFile::GetBuffer() {
	return (buffer_);
}

void MemFile::Clear() {
	ScopeLock lock(&lock_);
	current_pos_ = 0;
	size_ = 0;
}

int64 MemFile::GetSize() const {
	return ((int64)size_);
}

void MemFile::Resize(size_t size) {
	ScopeLock lock(&lock_);
	if (size >= buffer_size_) {
		const size_t new_buffer_size = size * 3 / 2;
		uint8* _buffer = new uint8[new_buffer_size];
		if (buffer_ != 0) {
			::memcpy(_buffer, buffer_, buffer_size_);
			delete[] buffer_;
		}
		buffer_ = _buffer;
		buffer_size_ = new_buffer_size;
	}

}

void MemFile::CropHead(size_t final_size) {
	ScopeLock lock(&lock_);
	deb_assert(final_size <= size_);
	if (final_size <= size_) {
		return;
	}
	size_t crop_byte_count = size_-final_size;
	::memmove(buffer_, buffer_+crop_byte_count, final_size);
	size_ = final_size;
	if (current_pos_ > crop_byte_count) {
		current_pos_ -= crop_byte_count;
	} else {
		current_pos_ = 0;
	}
	deb_assert(current_pos_ <= size_);
}

void* MemFile::GetBuffer(size_t minimum_size) {
	Resize(minimum_size);
	size_ = minimum_size;
	return buffer_;
}



int64 MemFile::GetAvailable() const {
	return ((int64)size_ - (int64)current_pos_);
}

IOError MemFile::ReadRaw(void* buffer, size_t size) {
	ScopeLock lock(&lock_);

	if (current_pos_ >= size_) {
		return kIoBufferUnderflow;
	}

	size_t end_pos = current_pos_ + size;
	if (end_pos > size_) {
		::memcpy(buffer, &buffer_[current_pos_], size_ - current_pos_);
		current_pos_ = size_;
		return kIoBufferUnderflow;
	}

	::memcpy(buffer, &buffer_[current_pos_], size);
	current_pos_ += size;

	return kIoOk;
}

IOError MemFile::Skip(size_t size) {
	return (Parent::Skip(size));
}

IOError MemFile::WriteRaw(const void* buffer, size_t size) {
	ScopeLock lock(&lock_);

	size_t end_pos = current_pos_ + size;

	Resize(end_pos);
	::memcpy(buffer_+current_pos_, buffer, size);
	current_pos_ += size;

	if (current_pos_ > size_) {
		size_ = current_pos_;
	}

	return (kIoOk);
}

void MemFile::Flush() {
	// Do nothing.
}

void MemFile::Close() {
	ScopeLock lock(&lock_);
	delete[] buffer_;
	buffer_ = 0;
}

IOError MemFile::ReadData(void* buffer, size_t size) {
	ScopeLock lock(&lock_);
	if (reader_ != 0) {
		return reader_->ReadData(buffer, size);
	} else {
		return Reader::ReadData(buffer, size);
	}
}

IOError MemFile::WriteData(const void* buffer, size_t size) {
	ScopeLock lock(&lock_);
	if (writer_ != 0) {
		return writer_->WriteData(buffer, size);
	} else {
		return Writer::WriteData(buffer, size);
	}
}



}
