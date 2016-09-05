/*
	Class:  BufferInputStream,
			BufferOutputStream
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/bufferstream.h"
#include <memory.h>

namespace lepra {

BufferInputStream::BufferInputStream(uint8* buffer, size_t buffer_size) :
	buffer_(buffer),
	buffer_size_(buffer_size),
	read_index_(0) {
	if (buffer_ == 0)
		buffer_size_ = 0;
}

BufferInputStream::~BufferInputStream() {
}

void BufferInputStream::Close() {
}

int64 BufferInputStream::GetAvailable() const {
	return (int64)(buffer_size_ - read_index_);
}

IOError BufferInputStream::ReadRaw(void* data, size_t length) {
	IOError error = kIoOk;
	size_t copy_length = length;

	if ((buffer_size_ - read_index_) < length) {
		copy_length = buffer_size_ - read_index_;
		error = kIoBufferUnderflow;
	}

	if (copy_length > 0) {
		memcpy(data, &buffer_[read_index_], copy_length);
		read_index_ += copy_length;
	}

	return error;
}

IOError BufferInputStream::Skip(size_t length) {
	read_index_ += length;
	if (read_index_ > buffer_size_) {
		read_index_ = buffer_size_;
		return kIoBufferUnderflow;
	}
	return kIoOk;
}




BufferOutputStream::BufferOutputStream(uint8* buffer, size_t buffer_size) :
	buffer_(buffer),
	buffer_size_(buffer_size),
	write_index_(0) {
	if (buffer_ == 0)
		buffer_size_ = 0;
}

BufferOutputStream::~BufferOutputStream() {
}

void BufferOutputStream::Close() {
}

void BufferOutputStream::Flush() {
}

IOError BufferOutputStream::WriteRaw(const void* data, size_t length) {
	IOError error = kIoOk;
	size_t copy_length = length;

	if ((buffer_size_ - write_index_) < length) {
		copy_length = buffer_size_ - write_index_;
		error = kIoBufferOverflow;
	}

	if (copy_length > 0) {
		memcpy(&buffer_[write_index_], data, copy_length);
		write_index_ += copy_length;
	}

	return error;
}

}
