#!/usr/bin/env python3

from select import select
import socket as _socket
from time import sleep

class socket:
	def __init__(self):
		self.s = _socket.socket(_socket.AF_INET, _socket.SOCK_DGRAM)
		self.s.setblocking(0)
		self.timeout = 0.2

	def connect(self, hostport):
		self.hostport = (_socket.gethostbyname(hostport[0]), hostport[1])

	def close(self):
		try:
			self.send('disconnect\n'.encode())
			self.s.shutdown(_socket.SHUT_RDWR)
			time.sleep(0.01)
		except:
			pass
		return self.s.close()

	def settimeout(self, t):
		self.timeout = t

	def send(self, data):
		if self.s.sendto(data, self.hostport) != len(data):
			raise _socket.error('unable to send')

	def recv(self, l):
		if self.timeout != None:
			r,_,e = select([self.s],[],[self.s], self.timeout)
			if e: raise _socket.error('disconnected')
			if not r: raise _socket.error('timeout')
		data,hostport = self.s.recvfrom(l)
		if hostport == self.hostport:
			if data == b'disconnect\n':
				raise _socket.error('disconnected')
			return data
