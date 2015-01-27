#!/usr/bin/env python3

import socket as _socket

class socket:
	def __init__(self):
		self.s = _socket.socket(_socket.AF_INET, _socket.SOCK_DGRAM)

	def connect(self, hostport):
		self.hostport = (_socket.gethostbyname(hostport[0]), hostport[1])
		self.s.settimeout(1)

	def close(self):
		try:	self.send('disconnect\n'.encode())
		except:	pass
		self.s.shutdown(_socket.SHUT_RDWR)
		return self.s.close()

	def send(self, data):
		if self.s.sendto(data, self.hostport) != len(data):
			raise _socket.error('unable to send')

	def recv(self, l):
		data,hostport = self.s.recvfrom(l)
		if hostport == self.hostport:
			return data
