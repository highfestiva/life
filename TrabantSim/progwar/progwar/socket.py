from select import select
import socket as _socket
from time import sleep

proto_version = 'v0.1'

class socket:
    def __init__(self):
        self.s = _socket.socket(_socket.AF_INET, _socket.SOCK_DGRAM)
        self.addr = None
        self.s.setblocking(0)
        self.timeout = 0

    def login(self, addr, user, password, color):
        self.addr = (_socket.gethostbyname(addr[0]), addr[1])
        try:
            to = self.timeout
            self.timeout = 5
            self.send('connect %s %s %s %s' % (proto_version,user,password,color))
            data,addr = self.recv()
            self.timeout = to
            return data,addr
        except:
            return 'could not connect to server',None

    def bind(self, addr):
        self.s.bind(addr)

    @staticmethod
    def gethostname():
        return _socket.gethostname()

    def close(self):
        try:
            self.send('disconnect')
            self.s.shutdown(_socket.SHUT_RDWR)
            time.sleep(0.01)
        except:
            pass
        return self.s.close()

    def send(self, data):
        self.sendto(data, self.addr)

    def sendto(self, data, addr):
        data = data.encode()
        if self.s.sendto(data, addr) != len(data):
            raise _socket.error('unable to send')

    def recv(self):
        if self.timeout != None:
            r,_,e = select([self.s],[],[self.s], self.timeout)
            if e: raise _socket.error('disconnected')
            if not r:
                if self.timeout:
                    raise _socket.error('timeout')
                return None,None
        data,addr = self.s.recvfrom(1400)
        if not self.addr or addr == self.addr:
            data = data.decode()
            if data == 'disconnect':
                raise _socket.error('disconnected')
            return data,addr
        return None,None
