/*
	Class:  SocketInputStream,
			SocketOutputStream
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/socketstream.h"
#include "../include/socket.h"

namespace lepra {

SocketInputStream::SocketInputStream(TcpSocket* socket) :
	socket_(socket) {
}

SocketInputStream::~SocketInputStream() {
}

void SocketInputStream::Close() {
	socket_->Disconnect();
}

int64 SocketInputStream::GetAvailable() const {
	return -1; // Unknown size...
}

IOError SocketInputStream::ReadRaw(void* data, size_t length) {
	unsigned sum = 0;
	unsigned read = 0;

	while (sum < length) {
		// TODO: Possible hacker attack? What if someone starts sending data,
		// but suddenly stops in the middle of something? The connection will
		// never shut down, and no data will ever arrive. Several such connections
		// will occupy a whole server and make it useless.
		read = socket_->Receive(&((uint8*)data)[sum], (int)(length-sum));

		if (read <= 0)
			return kIoStreamNotOpen;

		sum += read;
	}

	return kIoOk;
}

IOError SocketInputStream::Skip(size_t length) {
	uint8 buffer[Datagram::kBufferSize];
	size_t count = length / Datagram::kBufferSize;
	IOError io_error;

	for (unsigned i = 0; i < count; i++) {
		io_error = ReadRaw(buffer, Datagram::kBufferSize);
		if (io_error != kIoOk)
			return io_error;
	}

	return ReadRaw(buffer, length - count * Datagram::kBufferSize);
}


SocketOutputStream::SocketOutputStream(TcpSocket* socket) :
	socket_(socket) {
}

SocketOutputStream::~SocketOutputStream() {
}

void SocketOutputStream::Close() {
	socket_->Disconnect();
}

void SocketOutputStream::Flush() {
	// Do nothing.
}

IOError SocketOutputStream::WriteRaw(const void* data, size_t length) {
	if (socket_->IsOpen() == true) {
		socket_->Send((uint8*)data, (int)length);
		return kIoOk;
	}
	return kIoStreamNotOpen;
}

}
