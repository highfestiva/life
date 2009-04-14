/*
	Class:  SocketInputStream,
			SocketOutputStream
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
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
	virtual IOError WriteRaw(void* pData, size_t pLength);
protected:
private:
	TcpSocket* mSocket;
};

} // End namespace.
