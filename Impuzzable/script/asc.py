#!/usr/bin/env python3

import codecs
from collections import UserList
from copy import deepcopy
from vec3 import vec3


W2E, NW2SE, N2S, NE2SW, E2W, SE2NW, S2N, SW2NE, F2B, B2F, NORMAL_COUNT = range(11)
chars = ' X><^v`´,/|*ltrb'
square_triangles =	[	[[False,False],[False,False]],
				[[True ,True ],[True ,True ]],
				[[True ,False],[False,False]],
				[[False,False],[False,True ]],
				[[False,False],[True ,False]],
				[[False,True ],[False,False]],
				[[True ,True ],[False,False]],
				[[False,True ],[False,True ]],
				[[True ,False],[True ,False]],
				[[False,False],[True ,True ]],
				[[False,True ],[True ,False]],
				[[True ,False],[False,True ]],
				[[False,True ],[True ,True ]],
				[[True ,False],[True ,True ]],
				[[True ,True ],[True ,False]],
				[[True ,True ],[False,True ]],	]


class Shape:
	def __init__(self,original):
		if hasattr(original,'triangles'):
			self.size = original.size
			self.tricnt = original.tricnt
			self.triangles = deepcopy(original.triangles)
		else:
			chars = original
			self.size = vec3(len(chars[0][0])*2,len(chars[0])*2,len(chars))
			self.tricnt = 0
			self.triangles = []
			for z,layer in enumerate(chars):
				tlayer = []
				self.triangles.append(tlayer)
				for y,row in enumerate(layer):
					tlayer += [[False]*self.size.x,[False]*self.size.x]	# Add two triangle rows per char row.
					for x,ch in enumerate(row):
						self.tricnt += _ch2tricnt(ch)
						ts = _ch2tris(ch)
						self.triangles[z][y*2  ][x*2]	= ts[0][0]
						self.triangles[z][y*2  ][x*2+1]	= ts[0][1]
						self.triangles[z][y*2+1][x*2]	= ts[1][0]
						self.triangles[z][y*2+1][x*2+1]	= ts[1][1]
	def chars(self):
		cs = []
		for z,tlayer in enumerate(self.triangles):
			layer = []
			cs.append(layer)
			for y in range(self.size.y//2):
				row = ''
				for x in range(self.size.x//2):
					ts = [	[self.triangles[z][y*2  ][x*2], self.triangles[z][y*2  ][x*2+1]],
						[self.triangles[z][y*2+1][x*2], self.triangles[z][y*2+1][x*2+1]],  ]
					row += _tris2ch(ts)
				layer.append(row)
		return cs
	def __str__(self):
		return shape2str(self)


def _ch2tricnt(ch):
	if ch == ' ':      return 0
	if ch == 'X':      return 4
	if ch in '><^v':   return 1
	if ch in '`´,/*|': return 2
	if ch in 'ltrb':   return 3
	raise ValueError('Invalid character %s' % repr(ch))

def _ch2tris(ch):
	return square_triangles[chars.index(ch)]

def _tris2ch(tris):
	return chars[square_triangles.index(tris)]

def _settri(shape,crd,set):
	if shape.triangles[crd.z][crd.y][crd.x] == set:
		print(shape)
		raise ValueError('Trying to %s %s again!' % ('set' if set else 'clear', str(crd)))
	shape.triangles[crd.z][crd.y][crd.x] = set
	shape.tricnt += 1 if set else -1

def _validcrd(shape,crd):
	if crd.x<0 or crd.y<0 or crd.z<0:
		return False
	return crd.x<shape.size.x and crd.y<shape.size.y and crd.z<shape.size.z

def _drop_empty_head_tail(chars):
	o,chars = None,list(chars)
	while o != chars:
		o = chars
		if not list(filter(lambda s:s.strip(),chars[0])):
			chars = chars[1:]
		if not list(filter(lambda s:s.strip(),chars[-1])):
			chars = chars[:-1]
	return chars

def _get_topmost_tri(shape):
	for z,layer in enumerate(shape.triangles):
		for y,row in enumerate(layer):
			for x,t in enumerate(row):
				if t:
					return vec3(x,y,z)
	return None


def tricnt(shape):
	return shape.tricnt

def trimaxsize(shape):
	return shape.size

def hastri(shape,crd):
	return shape.triangles[crd.z][crd.y][crd.x]


def idx2crd(size,idx):
	z = idx/(size.x*size.y)
	idx %= size.x*size.y
	y = idx/size.x
	x = idx%size.x
	return vec3(x,y,z)

def addtri(shape,crd):
	_settri(shape,crd,True)

def droptri(shape,crd):
	_settri(shape,crd,False)

def merge_shapes(from_shape, to_shape):
	for z in range(from_shape.size.z):
		for y in range(from_shape.size.y):
			for x in range(from_shape.size.x):
				if from_shape.triangles[z][y][x]:
					if to_shape.triangles[z][y][x]:
						print(from_shape)
						print(to_shape)
						raise ValueError('Trying to merge, but overlaps in %s!' % vec3(x,y,z))
					to_shape.triangles[z][y][x] = True
					to_shape.tricnt += 1

def flood_fill_move(crd, from_shape, to_shape=None):
	if not to_shape:
		to_shape = new_shape(from_shape.size)
	if not hastri(from_shape,crd):
		return to_shape
	droptri(from_shape,crd)
	addtri(to_shape,crd)
	crds = get_neighbour_crds(from_shape,crd)
	for c in crds:
		flood_fill_move(c,from_shape,to_shape)
	return to_shape

def flood_fill_layer(crd, shape):
	if hastri(shape,crd):
		return
	addtri(shape,crd)
	crds = get_neighbour_crds(shape,crd,False,False)
	for crd in crds:
		flood_fill_layer(crd,shape)

def get_layer_holes(shape):
	'''Returns coordinates of all holes in all layers.'''
	# Clone shape, flood fill all triangles on the outline. The holes are all the coordinates without triangles.
	s = clone_shape(shape)
	# Fill layers.
	for z in range(s.size.z):
		for x in range(s.size.x//2):	# top
			if not s.triangles[z][0][x*2+1]:
				flood_fill_layer(vec3(x*2+1,0,z),s)
		bottom = s.size.y - 1		# bottom
		for x in range(s.size.x//2):
			if not s.triangles[z][bottom][x*2]:
				flood_fill_layer(vec3(x*2,bottom,z),s)
		for y in range(s.size.y//2):	# left
			if not s.triangles[z][y*2][0]:
				flood_fill_layer(vec3(0,y*2,z),s)
		right = s.size.x - 1		# right
		for y in range(s.size.y//2):
			if not s.triangles[z][y*2+1][right]:
				flood_fill_layer(vec3(right,y*2+1,z),s)
	holes = []
	for z,layer in enumerate(s.triangles):
		for y,row in enumerate(layer):
			for x,t in enumerate(row):
				if not t:
					holes += [vec3(x,y,z)]
	return holes

def get_split_shapes(shape):
	parts = []
	while True:
		crd = _get_topmost_tri(shape)
		if not crd:
			break
		parts += [flood_fill_move(crd,shape)]
	return parts

def clear_layer(shape,layeridx):
	shape.triangles[layeridx] = [[False]*shape.size.x for _ in range(shape.size.y)]

def split_layers(shape):
	'''Returns layers in a list, each layer as a separate shape. Empty layers get a None list entry instead of a shape.'''
	layers = []
	for z,layer in enumerate(shape.triangles):
		if list(filter(lambda r:list(filter(None,r)), layer)):
			clone = clone_shape(shape)
			layers += [clone]
			for idx in range(shape.size.z):
				if idx == z: continue
				clear_layer(clone,idx)
		else:
			layers += [None]
	return layers

def getcrds(shape):
	crds = set()
	for z in range(shape.size.z):
		for y in range(shape.size.y):
			for x in range(shape.size.x):
				if shape.triangles[z][y][x]:
					crds.add(vec3(x,y,z))
	return crds

def get_neighbour_crds(shape,crds,isset=True,allowz=True):
	crds = crds if hasattr(crds,'__contains__') else [crds]
	ns = set()
	for crd in crds:
		if crd.x&1 == 0 and crd.y&1 == 0:	# Left triangle (0).
			neighbours = [vec3(+1, 0,0),vec3( 0,+1,0),vec3(-1,+1,0)]
		elif crd.x&1 == 1 and crd.y&1 == 0:	# Top triangle (1).
			neighbours = [vec3(-1, 0,0),vec3( 0,+1,0),vec3(-1,-1,0)]
		elif crd.x&1 == 1 and crd.y&1 == 1:	# Right triangle (2).
			neighbours = [vec3( 0,-1,0),vec3(-1, 0,0),vec3(+1,-1,0)]
		else:					# Bottom triangle (3).
			neighbours = [vec3( 0,-1,0),vec3(+1, 0,0),vec3(+1,+1,0)]
		if allowz:
			neighbours += [vec3(0,0,-1),vec3(0,0,+1)]
		neighbours = [crd+d for d in neighbours]
		neighbours = [c for c in neighbours if _validcrd(shape,c) and c not in ns]
		ns.update([c for c in neighbours if hastri(shape,c)==isset])
	return ns

def get_normal(p,q):
	v = q-p
	if v.z > 0: return F2B
	if v.z < 0: return B2F
	if v == vec3(+1, 0,0): return SW2NE
	if v == vec3(-1, 0,0): return NE2SW
	if v == vec3( 0,+1,0): return NW2SE
	if v == vec3( 0,-1,0): return SE2NW
	if v == vec3(+1,+1,0) and p.x&1==0 and p.y&1==1: return N2S
	if v == vec3(-1,-1,0) and p.x&1==1 and p.y&1==0: return S2N
	if v == vec3(+1,-1,0) and p.x&1==1 and p.y&1==1: return W2E
	if v == vec3(-1,+1,0) and p.x&1==0 and p.y&1==0: return E2W
	raise ValueError('%s and %s are not neighbours?!' % (p,q))

def new_shape(size):
	return Shape([[' '*(size.x//2)]*(size.y//2) for _ in range(size.z)])

def clone_shape(shape):
	return Shape(shape)

def crop_chars(chars):
	tallest_layer = max(len(layer) for layer in chars)
	longest_line = max(len(row) for layer in chars for row in layer)
	chars = [layer+['']*(tallest_layer-len(layer)) for layer in chars]	# Make all layers equally tall.
	chars = [[line.ljust(longest_line) for line in layer] for layer in chars]	# Make all rows equally long.
	chars = _drop_empty_head_tail(chars)		# Drop empty layers in front and back.
	xpose = _drop_empty_head_tail(zip(*chars))	# Transpose layers ('deep' instead of 'high'), drop empty rows on top and bottom.
	xpose = [[''.join(s) for s in zip(*layer)] for layer in zip(*xpose)]	# Transpose back to front-to-back layers, then turn rows into columns.
	xpose = _drop_empty_head_tail(zip(*xpose))	# Transform to place all columns together, drop empty left-most columns.
	chars = [[''.join(s) for s in zip(*cols)] for cols in zip(*xpose)]	# Transform back to layers, columns turn back into rows.
	return chars


def load_shape(ascii):
	chars = []	# List of layers.
	layer = []	# List of strings. One string per row.
	for line in codecs.open(ascii+'.txt', encoding='utf-8'):
		line = line.rstrip()
		if '---' in line:
			chars += [layer]
			layer = []
		else:
			layer += [line.rstrip('\n')]
	chars += [layer]
	return Shape(crop_chars(chars))


def shape2str(shape):
	splitter = '\n' + '-'*max(3,shape.size.x//2) + '\n'
	return splitter.join(['\n'.join(layer) for layer in shape.chars()])
