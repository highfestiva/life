
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/writer.h"
#include "../include/outputstream.h"



namespace lepra {



Writer::Writer(Endian::EndianType endian):
	out_stream_(0),
	writer_endian_(endian) {
}

Writer::Writer(OutputStream* out, Endian::EndianType endian):
	out_stream_(out),
	writer_endian_(endian) {
}

Writer::~Writer() {
}

IOError Writer::WriteData(const void* buffer, size_t size) {
	return out_stream_->WriteRaw(buffer, size);
}

const str& Writer::GetStreamName() {
	return out_stream_->GetName();
}

IOError Writer::Write(const char& data) {
	return (WriteData(&data, sizeof(data)));
}

IOError Writer::Write(const wchar_t& data) {
	return (WriteData(&data, sizeof(data)));
}

IOError Writer::Write(const int8& data) {
	return WriteData(&data, sizeof(int8));
}

IOError Writer::Write(const uint8& data) {
	return WriteData(&data, sizeof(uint8));
}

IOError Writer::Write(const int16& data) {
	int16 _data = Endian::HostTo(writer_endian_, data);
	return WriteData(&_data, sizeof(int16));
}

IOError Writer::Write(const uint16& data) {
	uint16 _data = Endian::HostTo(writer_endian_, data);
	return WriteData(&_data, sizeof(uint16));
}

IOError Writer::Write(const int32& data) {
	int _data = Endian::HostTo(writer_endian_, data);
	return WriteData(&_data, sizeof(int));
}

IOError Writer::Write(const uint32& data) {
	unsigned _data = Endian::HostTo(writer_endian_, data);
	return WriteData(&_data, sizeof(unsigned));
}

IOError Writer::Write(const int64& data) {
	int64 _data = Endian::HostTo(writer_endian_, data);
	return WriteData(&_data, sizeof(int64));
}

IOError Writer::Write(const uint64& data) {
	uint64 _data = Endian::HostTo(writer_endian_, data);
	return WriteData(&_data, sizeof(uint64));
}

IOError Writer::Write(const float32& data) {
	float32 _data = Endian::HostTo(writer_endian_, data);
	return WriteData(&_data, sizeof(float32));
}

IOError Writer::Write(const float64& data) {
	float64 _data = Endian::HostTo(writer_endian_, data);
	return WriteData(&_data, sizeof(float64));
}

void Writer::SetWriterEndian(Endian::EndianType writer_endian) {
	writer_endian_ = writer_endian;
}

Endian::EndianType Writer::GetWriterEndian() const {
	return writer_endian_;
}

IOError Writer::WriteString(const str& s) {
	return (WriteData(s.c_str(), (unsigned)s.length()));
}

void Writer::SetOutputStream(OutputStream* out_stream) {
	out_stream_ = out_stream;
}



}
