
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/subfile.h"



namespace lepra {



SubFile::SubFile(File* master_file, int64 file_size) :
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian),
	master_file_(master_file),
	file_size_(file_size) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
	file_start_ = master_file_->Tell();
}

SubFile::~SubFile() {
}



void SubFile::Flush() {
}

void SubFile::Close() {
}

int64 SubFile::GetAvailable() const {
	int64 _offset = Tell()-file_start_;
	return (file_size_-_offset);
}

IOError SubFile::Skip(size_t length) {
	return (File::Skip(length));
}

IOError SubFile::ReadRaw(void* data, size_t length) {
	IOError status = kIoBufferOverflow;
	int64 end_offset = Tell()+length;
	if (file_size_ >= end_offset) {
		status = master_file_->ReadData(data, length);
	}
	return (status);
}

IOError SubFile::WriteRaw(const void* data, size_t length) {
	IOError status = kIoBufferOverflow;
	int64 end_offset = Tell()+length;
	if (file_size_ >= end_offset) {
		status = master_file_->WriteData(data, length);
	}
	return (status);
}



int64 SubFile::GetSize() const {
	return (file_size_);
}

int64 SubFile::Tell() const {
	int64 _offset = master_file_->Tell()-file_start_;
	return (_offset);
}

int64 SubFile::Seek(int64 offset, FileOrigin from) {
	int64 _offset = 0;
	switch(from) {
		case kFseekSet:	_offset = offset;		break;
		case kFseekCur:	_offset = Tell()+offset;	break;
		case kFseekEnd:	_offset = file_size_-offset;	break;
	};
	if (_offset < 0) {
		_offset = 0;
	} else if (_offset > file_size_) {
		_offset = file_size_;
	}
	_offset = master_file_->SeekSet(_offset+file_start_);
	return (_offset);
}



}
