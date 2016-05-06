/*
	Class:  SocketInputStream,
			SocketOutputStream
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "inputstream.h"
#include "outputstream.h"

namespace lepra {

class TcpSocket;

class SocketInputStream : public InputStream {
public:
	SocketInputStream(TcpSocket* socket);
	virtual ~SocketInputStream();

	virtual void Close();
	virtual int64 GetAvailable() const;
	virtual IOError ReadRaw(void* data, size_t length);
	virtual IOError Skip(size_t length);
protected:
private:
	TcpSocket* socket_;
};

class SocketOutputStream : public OutputStream {
public:
	SocketOutputStream(TcpSocket* socket);
	virtual ~SocketOutputStream();

	virtual void Close();
	virtual void Flush();
	virtual IOError WriteRaw(const void* data, size_t length);
protected:
private:
	TcpSocket* socket_;
};

}
