/*
	Class:  SocketInputStream,
			SocketOutputStream
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../Include/SocketStream.h"
#include "../Include/Socket.h"

namespace Lepra
{

SocketInputStream::SocketInputStream(TcpSocket* pSocket) :
	mSocket(pSocket)
{
}

SocketInputStream::~SocketInputStream()
{
}

void SocketInputStream::Close()
{
	mSocket->Disconnect();
}

int64 SocketInputStream::GetAvailable() const
{
	return -1; // Unknown size...
}

IOError SocketInputStream::ReadRaw(void* pData, size_t pLength)
{
	unsigned lSum = 0;
	unsigned lRead = 0;

	while (lSum < pLength)
	{
		// TODO: Possible hacker attack? What if someone starts sending data,
		// but suddenly stops in the middle of something? The connection will 
		// never shut down, and no data will ever arrive. Several such connections
		// will occupy a whole server and make it useless.
		lRead = mSocket->Receive(&((uint8*)pData)[lSum], (int)(pLength-lSum));

		if (lRead <= 0)
			return IO_STREAM_NOT_OPEN;

		lSum += lRead;
	}

	return IO_OK;
}

IOError SocketInputStream::Skip(size_t pLength)
{
	uint8 lBuffer[Datagram::BUFFER_SIZE];
	size_t lCount = pLength / Datagram::BUFFER_SIZE;
	IOError lIOError;

	for (unsigned i = 0; i < lCount; i++)
	{
		lIOError = ReadRaw(lBuffer, Datagram::BUFFER_SIZE);
		if (lIOError != IO_OK)
			return lIOError;
	}

	return ReadRaw(lBuffer, pLength - lCount * Datagram::BUFFER_SIZE);
}


SocketOutputStream::SocketOutputStream(TcpSocket* pSocket) :
	mSocket(pSocket)
{
}

SocketOutputStream::~SocketOutputStream()
{
}

void SocketOutputStream::Close()
{
	mSocket->Disconnect();
}

void SocketOutputStream::Flush()
{
	// Do nothing.
}

IOError SocketOutputStream::WriteRaw(const void* pData, size_t pLength)
{
	if (mSocket->IsOpen() == true)
	{
		mSocket->Send((uint8*)pData, (int)pLength);
		return IO_OK;
	}
	return IO_STREAM_NOT_OPEN;
}

} // End namespace.
