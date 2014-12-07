#!/usr/bin/env python3

import socket
import time
from quat import quat
from vec3 import vec3

sock = None
proc = None
joint_terminate = True

def opencom(addr, connect_retrys=3, retry_intermission=1):
	global sock,proc
	if sock:
		return
	if 'localhost:' in addr:
		import os.path
		import subprocess
		path = os.path.abspath('.').replace('\\','/')
		while '/' in path and not proc:
			try:
				proc = subprocess.Popen([os.path.join(path,'bin/tv3d.exe'), addr], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
			except:
				path = path.rsplit('/',1)[0]
		if not proc:
			print('tv3d could not be started.')
	import atexit
	import signal
	def ctrlc(s,f):
		closecom()
		import sys
		sys.exit(0)
	signal.signal(signal.SIGINT, ctrlc)
	atexit.register(closecom)
	import socket
	ip,port = addr.split(':')
	for _ in range(connect_retrys):
		try:
			sock = socket.socket()
			sock.connect((ip,int(port)))
			break
		except socket.error:
			sock = None
			time.sleep(retry_intermission)
	if not sock:
		print('tv3d not available through tcp on %s.' % addr)

def open(camdist=3, camangle=(0,0,0), camrotspeed=(0,0,0.1), fov=60, bg='#000', fg='#b59', fps=30, addr='localhost:2541'):
	global joint_terminate
	joint_terminate = 'localhost:' in addr
	opencom(addr=addr)
	setcam(dist=camdist, angle=camangle, rotspeed=camrotspeed, fov=fov)
	r,g,b = _htmlcol(bg)
	set('Ui.3D.ClearRed', r)
	set('Ui.3D.ClearGreen', g)
	set('Ui.3D.ClearBlue', b)
	r,g,b = _htmlcol(fg)
	set('Ui.3D.PenRed', r)
	set('Ui.3D.PenGreen', g)
	set('Ui.3D.PenBlue', b)
	set('Physics.FPS', int(fps))

def closecom():
	global sock,proc,joint_terminate
	if sock:
		if joint_terminate:
			cmd('quit')
		sock.close()
		sock = None
	if proc and joint_terminate:
		print('closing tv3d')
		proc.kill()
		proc.wait()
		proc = None

def opened():
	return sock != None

def releaseobjects():
	cmd('delete-objects')

def creategfxobject(vertices,indices):
	initgfxmesh(quat(1,0,0,0),vec3(0,0,0),vertices,indices)
	createobject()

def createobject():
	cmd('create-object')

def clearphys():
	cmd('clear-phys')

def initphysbox(q,pos,size):
	cmd('prep-phys-box %f %f %f %f %f %f %f %f %f %f' % tuple((pq for pq in q[:]+pos[:]+size[:])))

def initphysmesh(q,pos,vertices,indices):
	setmesh(vertices,indices)
	prepphysmesh()

def initgfxmesh(q,pos,vertices,indices):
	setmesh(vertices,indices)
	prepgfxmesh(quat(1,0,0,0),vec3(0,0,0))

def prepphysmesh(q,pos):
	cmd('prep-phys-mesh %f %f %f %f %f %f %f' % tuple((pq for pq in q[:]+pos[:])))

def prepgfxmesh(q,pos):
	cmd('prep-gfx-mesh %f %f %f %f %f %f %f' % tuple((pq for pq in q[:]+pos[:])))

def setmesh(vertices, indices):
	cmd('set-vertices %s' % ' '.join((str(v) for v in vertices)))
	cmd('set-indices %s' % ' '.join((str(i) for i in indices)))

def setcam(lookat=(0,0,0), angle=(0,0,0), slide=0.5, dist=3, rotspeed=(0,0,0.1), fov=60):
	set('Ui.3D.CamLookX', float(lookat[0]))
	set('Ui.3D.CamLookY', float(lookat[1]))
	set('Ui.3D.CamLookZ', float(lookat[2]))
	set('Ui.3D.CamSlide', float(slide))
	set('Ui.3D.CamDistance', float(dist))
	set('Ui.3D.ClipFar', float(dist*20))
	set('Ui.3D.CamAngleX', float(angle[0]))
	set('Ui.3D.CamAngleY', float(angle[1]))
	set('Ui.3D.CamAngleZ', float(angle[2]))
	set('Ui.3D.CamRotateX', float(rotspeed[0]))
	set('Ui.3D.CamRotateY', float(rotspeed[1]))
	set('Ui.3D.CamRotateZ', float(rotspeed[2]))
	set('Ui.3D.FOV', float(fov))

def sleep(t):	# Convenience
	if sock:
		time.sleep(t)

def testdraw(vertices,indices):
	'Implement this in your test code.'
	pass

def set(setting, value):
	cmd('#%s %s' % (setting, str(value)))

def cmd(s):
	global sock
	if sock and s:
		try:
			sock.send((s+'\n').encode())
			return sock.recv(1024).decode()
		except socket.error as e:
			print(e)
			sock = None

def _htmlcol(col):
	assert col[0]=='#'
	return [int(c,16)/16 for c in col[1:]]
