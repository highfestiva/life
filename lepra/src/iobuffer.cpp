
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/iobuffer.h"



namespace lepra {



IOBuffer::IOBuffer(size_t max_size) :
	max_size_(max_size),
	buffer_(0),
	buffer_size_(0),
	buffer_start_(0),
	data_size_(0),
	buffer_owner_(true) {
}

IOBuffer::IOBuffer(uint8* buffer, size_t size, size_t data_size) :
	max_size_(size),
	buffer_(buffer),
	buffer_size_(size),
	buffer_start_(0),
	data_size_(data_size),
	buffer_owner_(false) {
}

IOBuffer::~IOBuffer() {
	if (buffer_owner_ == true && buffer_ != 0) {
		delete[] buffer_;
	}
}

void IOBuffer::Close() {
	// Do nothing...
}

void IOBuffer::Flush() {
	data_size_ = 0;
}

IOError IOBuffer::WriteRaw(const void* data, size_t length) {
	if (buffer_owner_ == true && (data_size_ + length) > buffer_size_) {
		ExtendDataBuffer(data_size_ + length);
	}

	IOError err = kIoOk;

	if ((data_size_ + length) > buffer_size_) {
		length = buffer_size_ - data_size_;
		err = kIoBufferOverflow;
	}

	// Calculate the starting position for writing (buffer_start_ is for reading).
	size_t buffer_start = (buffer_start_ + data_size_) % buffer_size_;

	if (buffer_start + length > buffer_size_) {
		size_t _size = buffer_size_ - buffer_start;
		memcpy(&buffer_[buffer_start],
			   data,
			   (size_t)_size);
		memcpy(buffer_,
			   &((uint8*)data)[_size],
			   (size_t)(length - _size));
	} else {
		memcpy(&buffer_[buffer_start], data, (size_t)length);
	}

	data_size_ += length;

	return err;
}

IOError IOBuffer::ReadRaw(void* data, size_t length) {
	IOError err = kIoOk;

	if (length > data_size_) {
		length = data_size_;
		err = kIoBufferUnderflow;
	}

	if (buffer_start_ + length > buffer_size_) {
		size_t _size = buffer_size_ - buffer_start_;
		memcpy(data,
			   &buffer_[buffer_start_],
			   (size_t)_size);
		memcpy(&((uint8*)data)[_size],
			   buffer_,
			   (size_t)(length - _size));
	} else {
		memcpy(data, &buffer_[buffer_start_], (size_t)length);
	}

	buffer_start_ = (buffer_start_ + length) % buffer_size_;
	data_size_ -= length;

	return err;
}

int64 IOBuffer::GetAvailable() const {
	return (int64)data_size_;
}

IOError IOBuffer::Skip(size_t length) {
	if (length > data_size_) {
		length = data_size_;
	}

	buffer_start_ = (buffer_start_ + length) % buffer_size_;
	data_size_ -= length;

	return kIoOk;
}

void IOBuffer::ExtendDataBuffer(size_t min_size) {
	if (max_size_ != 0 && buffer_size_ >= max_size_) {
		return;
	}

	size_t new_size;

	if (buffer_size_ <= 1) {
		new_size = min_size;
	} else {
		// Increase buffer size 1.5 times.
		new_size = (buffer_size_ * 3) / 2;

		// If still less than the minimum size...
		if (new_size < min_size) {
			new_size = min_size;
		}
	}

	if (max_size_ != 0 && new_size > max_size_) {
		new_size = max_size_;
	}

	uint8* _buffer = new uint8[new_size];

	if (buffer_ != 0) {
		if (buffer_start_ + data_size_ > buffer_size_) {
			size_t _size = buffer_size_ - buffer_start_;
			memcpy(_buffer, &buffer_[buffer_start_], _size);
			memcpy(&_buffer[_size], buffer_, data_size_ - _size);
		} else {
			memcpy(_buffer, &buffer_[buffer_start_], data_size_);
		}
		delete[] buffer_;
	}

	buffer_ = _buffer;
	buffer_size_ = new_size;
	buffer_start_ = 0;
}

}
