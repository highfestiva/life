/*
	Class:  SocketInputStream,
			SocketOutputStream
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "InputStream.h"
#include "OutputStream.h"

namespace Lepra
{

class TcpSocket;

class SocketInputStream : public InputStream
{
public:
	SocketInputStream(TcpSocket* pSocket);
	virtual ~SocketInputStream();

	virtual void Close();
	virtual int64 GetAvailable() const;
	virtual IOError ReadRaw(void* pData, size_t pLength);
	virtual IOError Skip(size_t pLength);
protected:
private:
	TcpSocket* mSocket;
};

class SocketOutputStream : public OutputStream
{
public:
	SocketOutputStream(TcpSocket* pSocket);
	virtual ~SocketOutputStream();

	virtual void Close();
	virtual void Flush();
	virtual IOError WriteRaw(const void* pData, size_t pLength);
protected:
private:
	TcpSocket* mSocket;
};

}
