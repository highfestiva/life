
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "pch.h"
#include "../include/reader.h"
#include "../include/inputstream.h"



namespace lepra {



Reader::Reader() :
	read_count_(0),
	in_stream_(0),
	reader_endian_(Endian::kTypeBigEndian) {
}

Reader::Reader(Endian::EndianType endian) :
	read_count_(0),
	in_stream_(0),
	reader_endian_(endian) {
}

Reader::Reader(InputStream* in) :
	read_count_(0),
	in_stream_(in),
	reader_endian_(Endian::kTypeBigEndian) {
}

Reader::Reader(InputStream* in, Endian::EndianType endian) :
	read_count_(0),
	in_stream_(in),
	reader_endian_(endian) {
}

Reader::~Reader() {
}

IOError Reader::Read(char& data) {
	return (ReadData(&data, sizeof(data)));
}

IOError Reader::Read(wchar_t& data) {
	return (ReadData(&data, sizeof(data)));
}

IOError Reader::Read(int8& data) {
	return ReadData(&data, sizeof(int8));
}

IOError Reader::Read(uint8& data) {
	return ReadData(&data, sizeof(uint8));
}

IOError Reader::Read(int16& data) {
	int16 _data;
	IOError err = ReadData(&_data, sizeof(int16));
	data = Endian::HostTo(reader_endian_, _data);

	return err;
}

IOError Reader::Read(uint16& data) {
	uint16 _data;
	IOError err = ReadData(&_data, sizeof(uint16));
	data = Endian::HostTo(reader_endian_, _data);
	return err;
}

IOError Reader::Read(int32& data) {
	int _data;
	IOError err = ReadData(&_data, sizeof(int));
	data = Endian::HostTo(reader_endian_, _data);
	return err;
}

IOError Reader::Read(uint32& data) {
	unsigned _data;
	IOError err = ReadData(&_data, sizeof(unsigned));
	data = Endian::HostTo(reader_endian_, _data);
	return err;
}

IOError Reader::Read(int64& data) {
	int64 _data;
	IOError err = ReadData(&_data, sizeof(int64));
	data = Endian::HostTo(reader_endian_, _data);
	return err;
}

IOError Reader::Read(uint64& data) {
	uint64 _data;
	IOError err = ReadData(&_data, sizeof(uint64));
	data = Endian::HostTo(reader_endian_, _data);
	return err;
}

IOError Reader::Read(float32& data) {
	float32 _data;
	IOError err = ReadData(&_data, sizeof(float32));
	data = Endian::HostTo(reader_endian_, _data);
	return err;
}

IOError Reader::Read(float64& data) {
	float64 _data;
	IOError err = ReadData(&_data, sizeof(float64));
	data = Endian::HostTo(reader_endian_, _data);
	return err;
}

IOError Reader::ReadData(void* buffer, size_t size) {
	read_count_ += (uint64)size;
	return in_stream_->ReadRaw(buffer, size);
}

IOError Reader::AllocReadData(void** buffer, size_t size) {
	char* _data = new char[size];
	IOError status = ReadData(_data, size);
	if (status == kIoOk) {
		*buffer = (void*)_data;
	} else {
		delete[] (_data);
	}
	return (status);
}

IOError Reader::Skip(size_t size) {
	return in_stream_->Skip(size);
}

void Reader::SetReaderEndian(Endian::EndianType reader_endian) {
	reader_endian_ = reader_endian;
}

Endian::EndianType Reader::GetReaderEndian() const {
	return reader_endian_;
}

const str& Reader::GetStreamName() {
	return in_stream_->GetName();
}

int64 Reader::GetAvailable() const {
	return in_stream_->GetAvailable();
}

uint64 Reader::GetReadCount() {
	return read_count_;
}

IOError Reader::ReadLine(str& s) {
	s.clear();
	char c = '\0';
	IOError err = Read(c);
	if (err != kIoOk) {
		return (err);
	}
	while (c != '\0' && c != '\n') {
		if (c != '\r') {
			s += c;
		}
		err = Read(c);
		if (err != kIoOk) {
			if (err == kIoErrorReadingFromStream) {
				break;	// We'll take the error on the next (=empty) line.
			}
			return (err);
		}
	}

	return kIoOk;
}

void Reader::SetInputStream(InputStream* in_stream) {
	read_count_ = 0;
	in_stream_ = in_stream;
}



}
