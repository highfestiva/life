#!/usr/bin/env python3

import codecs
import glob
import merge3
import os
import os.path
import socket
import struct


remotepath = 'remote/'
localpath = '../prototypes/'
storepath = 'store_%s/'
EQUAL,BOTHSAME,LOCALMERGE,REMOTEMERGE = '= ~ < >'.split()
nopath = lambda filename: filename.replace('\\','/').split('/')[-1]
nopathset = lambda files: set(nopath(f) for f in files)
_glob = lambda wildcard: [f.replace('\\','/') for f in glob.glob(wildcard) if os.path.isfile(f)]


def output(info):
	print(info)#, flush=True)


def readcmd(sock):
	cmd = sock.recv(1+4)
	arglen = struct.unpack('!I', cmd[1:])[0]
	data = b''
	while len(data) < arglen:
		data += sock.recv(arglen-len(data))
	return chr(cmd[0]),data

def writecmd(sock, cmd, data):
	if type(data) == str:
		data = data.encode()
	raw = struct.pack('c', cmd.encode()) + struct.pack('!I', len(data))
	raw += data
	cnt = 0
	while cnt < len(raw):
		cnt += sock.send(raw[cnt:])


def writefile(filename, data):
	#print('writing data', data, 'to file', filename)
	if data:
		data = data.replace('\r\n','\n').replace('\t','    ')
		with codecs.open(filename, 'wb', 'utf-8') as f:
			f.write(data)
	elif os.path.exists(filename):
		os.remove(filename)

def updatefile(filename, data):
	if type(data) != str:
		data = data.decode(errors='replace')
	try:
		with codecs.open(filename, 'rb', 'utf-8') as f:
			prevdata = f.read().replace('\r\n','\n')
			if data and prevdata == data:
				return False
	except Exception as e:
		#print(e)
		pass
	writefile(filename, data)
	return True


def download(sock):
	old_files = _glob(remotepath+'*')
	writecmd(sock, 'p', '*')
	c,files = readcmd(sock)
	remote_files = files.decode(errors='ignore').split('\n')
	for file in remote_files:
		if remotepath+file in old_files:
			old_files.remove(remotepath+file)
		_,data = readcmd(sock)
		updatefile(remotepath+file, data)
	[os.remove(file) for file in old_files]
	return remote_files


def makedirs(*dirs):
	for d in dirs:
		try:	os.mkdir(d)
		except:	pass


def merge(basename, remote_hostname):
	local_hostname = socket.gethostname()
	preprocess = lambda s:s.replace('\t','    ')	# Replace spaces for best iOS experience.
	basefile = merge3.mergefile(storepath+basename, preprocess=preprocess)
	localfile = merge3.mergefile(localpath+basename, local_hostname, preprocess=preprocess)
	remotefile = merge3.mergefile(remotepath+basename, remote_hostname, preprocess=preprocess)
	base,local,remote,merged = merge3.merge3_full(basefile, localfile, remotefile)
	equality = EQUAL
	if base != merged:
		equality = BOTHSAME
	if not local or local != merged:
		writefile(localpath+basename, merged)
		equality = LOCALMERGE
	if not remote or remote != merged:
		writefile(remotepath+basename, merged)
		equality = REMOTEMERGE
	if not merged or equality != EQUAL:
		writefile(storepath+basename, merged)
	conflicted = ('<<<<<' in merged) and ('>>>>>' in merged)
	return equality,conflicted,merged


def sync(remote_addr, download_only=False):
	remote_addr = remote_addr.split(':')
	remote_hostname,port = remote_addr if len(remote_addr)==2 else (remote_addr[0],2541)
	sock = socket.socket()
	sock.settimeout(1)
	sock.connect((remote_hostname,int(port)))

	global storepath
	storepath = storepath % remote_hostname.lower()
	makedirs(remotepath, localpath, storepath)

	remote_files = download(sock)
	local_files = _glob(localpath+'*')
	store_files = _glob(storepath+'*')
	if download_only:
		output('Download of %i files done.' % len(remote_files))
		return

	all_files = nopathset(local_files) | nopathset(remote_files)

	# Probably not a good idea to clean out old storage. We use this as backup.
	#old_store_files = nopathset(store_files) - all_files
	#[os.remove(storepath+file) for file in old_store_files]

	# Merge and transmit those that changed.
	remote_files = []
	for file in sorted(all_files):
		equality,conflicted,data = merge(file, remote_hostname)
		if equality == REMOTEMERGE:
			remote_files += [file]
			writecmd(sock,'w',file)
			writecmd(sock,'b',data)
			conflictinfo = '(has unresolved merge conflicts)' if conflicted else ''
			output('%s: merged and uploaded %s' % (file, conflictinfo))
		elif equality == LOCALMERGE:
			conflictinfo = '(has conflicts)' if conflicted else ''
			output('%s: downloaded and merged %s' % (file, conflictinfo))
		elif conflicted:
			output('%s: still contains conflict' % file)
	# Read after all writes in order to reduce latency.
	for _ in remote_files:
		readcmd(sock)
	writecmd(sock, 'q', '')
	sock.shutdown(socket.SHUT_RDWR)
	sock.close()
	output('Synchronization with %s done.' % remote_hostname)


if __name__ == '__main__':
	import sys
	addr = sys.argv[-1] if len(sys.argv) > 1 else 'localhost:2541'
	sync(addr, '--download-only' in sys.argv)
