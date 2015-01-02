#!/usr/bin/env python3

# This dummy server is intended to be used if you need to test that your game prototype is
# functional, but you currently don't have access to any working binary server or the IDE.


import socket


host = 'localhost'
port = 2541
backlog = 5
size = 65536
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host,port))
s.listen(backlog)

counter = 1

def create_object(): return counter
def create_engine(): return counter
def create_joint(): return counter
def create_joystick(): return counter
def get_position(): return '0 0.1 0'
def get_velocity(): return '0 0.1 0'
def get_accelerometer(): return '0 9 0'

print('Dummy server up and running.')
while 1:
	client,address = s.accept()
	try:
		while True:
			data = client.recv(size).decode()
			result = ''
			try:
				f = data.split()[0]
				print(f)
				result = str(eval(f.replace('-','_')+'()'))
			except:
				pass
			client.send(('ok\n'+result).encode())
			counter += 1
	except:
		print('Client died. Listening again.')
	client.close()
