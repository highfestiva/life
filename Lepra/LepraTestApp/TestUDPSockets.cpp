/*
	File:   TestUDPSockets.cpp
	Class:  -
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../lepra/include/lepraassert.h"
#include "../include/log.h"
#include "../include/network.h"
#include "../include/socket.h"
#include "../include/string.h"
#include "../include/thread.h"
#include "../include/reader.h"
#include "../include/writer.h"

using namespace lepra;

void ReportTestResult(const lepra::LogDecorator& log, const str& test_name, const str& context, bool result);

class ServerThread : public Thread {
public:
	inline ServerThread():
		Thread("ServerThread"),
		udp_mux_socket_(0) {
		test_ok_ = false;

		SocketAddress local_address;
		if (local_address.Resolve(":10000")) {
			udp_mux_socket_ = new UdpMuxSocket("Srv ", local_address, true);
			deb_assert(udp_mux_socket_->IsOpen());
		}
	}

	inline ~ServerThread() {
		delete udp_mux_socket_;
	}

	inline bool GetTestOK() {
		return (test_ok_);
	}

protected:
	void Run();
private:

	UdpMuxSocket* udp_mux_socket_;
	bool test_ok_;
};

void ServerThread::Run() {
	UdpVSocket* socket = udp_mux_socket_->Accept();
	deb_assert(socket);
	Reader reader(socket);
	Writer writer(socket);

	socket->WaitAvailable(0.5);

	str s;
	reader.ReadLine(s);

//	printf("\nServer received \"%s\".", s.c_str());

	test_ok_ = (s == "Hi Server! I am Client!");
	deb_assert(test_ok_);

	writer.WriteString("Hi Client! I am Server!\n");
	socket->Flush();

	udp_mux_socket_->CloseSocket(socket);
}




class ClientThread : public Thread {
public:
	inline ClientThread() :
		Thread("ClientThread"),
		udp_mux_socket_(0) {
		test_ok_ = false;

		SocketAddress local_address;
		if (local_address.Resolve(":10001")) {
			udp_mux_socket_ = new UdpMuxSocket("Client ", local_address, false);
			deb_assert(udp_mux_socket_->IsOpen());
		}
	}

	inline ~ClientThread() {
		delete udp_mux_socket_;
	}

	inline bool GetTestOK() {
		return test_ok_;
	}

protected:
	void Run();
private:

	UdpMuxSocket* udp_mux_socket_;
	bool test_ok_;
};

void ClientThread::Run() {
	SocketAddress local_address;
	if (local_address.Resolve(":10000")) {
		UdpVSocket* socket = udp_mux_socket_->Connect(local_address, "", 0.5);
		deb_assert(socket);
		Reader reader(socket);
		Writer writer(socket);

		writer.WriteString("Hi Server! I am Client!\n");
		socket->Flush();

		socket->WaitAvailable(0.5);
		str s;
		reader.ReadLine(s);

	//	printf("Client received \"%s\".", s.c_str());

		test_ok_ = (s == "Hi Client! I am Server!");
		deb_assert(test_ok_);

		udp_mux_socket_->CloseSocket(socket);
	}
}





bool TestUDPSockets(const LogDecorator& account) {
	str _context;
	bool test_ok = Network::Start();

	if (test_ok) {
		ServerThread server;
		server.Start();

		ClientThread client;
		client.Start();

		server.GraceJoin(10.0);
		client.GraceJoin(10.0);

		_context = "Client/Server UdpMuxSocket communication failed";
		test_ok = client.GetTestOK() && server.GetTestOK();
		deb_assert(test_ok);

		Network::Stop();
	}

	ReportTestResult(account, "UDPSockets", _context, test_ok);
	return (test_ok);
}
