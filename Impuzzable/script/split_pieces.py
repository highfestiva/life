#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asc
import random


def take_piece(p):
	takecnt = asc.tricnt(p)//2
	size = asc.trimaxsize(p)
	order = list(range(size.x*size.y*size.z))
	random.shuffle(order)
	for idx in order:
		v = asc.idx2crd(size,idx)
		if asc.hastri(p,v):
			crd = v
			break
	t = asc.new_shape(size)
	asc.addtri(t,crd)
	asc.droptri(p,crd)
	usedcrds = set([crd])
	prev_crd = None
	for i in range(takecnt-1):
		crds = asc.get_neighbour_crds(p,crd)
		crds = [c for c in crds if c not in usedcrds]
		if random.random() < 0.98:
			crds = [c for c in crds if c.z==crd.z]
		if not crds:
			# Check if it's possible to continue on previous triangle.
			if not prev_crd:
				break
			crd,prev_crd = prev_crd,None
			continue
		random.shuffle(crds)
		prev_crd,crd = crd,crds[0]
		usedcrds.add(crd)
		asc.addtri(t,crd)
		asc.droptri(p,crd)
	crds = asc.get_layer_holes(t)
	for crd in crds:
		asc.addtri(t,crd)
		asc.droptri(p,crd)
	return [t,p]

def cleanup_layers(p):
	'''Each layer must only contain X coherent pieces.'''
	pieces = []
	layers = asc.split_layers(p)
	good_layers = [None] * len(layers)
	for idx,layer in enumerate(layers):
		if not layer: continue
		ps = asc.get_split_shapes(layer)
		if len(ps) == 1:
			good_layers[idx] = ps[0]
		else:
			pieces += ps
	merge_layer = None
	for good_layer in good_layers:
		if good_layer:
			if merge_layer:
				asc.merge_shapes(good_layer,merge_layer)
			else:
				merge_layer = good_layer
				pieces += [merge_layer]
		else:
			merge_layer = None
	return pieces

def open_cage(p, ps):
	crds = asc.getcrds(p)
	neighbours = asc.get_neighbour_crds(p,crds,False)
	for shape in [q for q in ps if q!=p]:
		if asc.tricnt(p) < asc.tricnt(shape):
			continue
		border_triangles = []
		for n in list(neighbours):
			if asc.hastri(shape,n):
				border_triangles += [n]
				neighbours.remove(n)
		normals = [False]*asc.NORMAL_COUNT
		for b in border_triangles:
			for c in asc.get_neighbour_crds(p,b):
				normals[asc.get_normal(b,c)] = True
		free2move,normals = False,normals[:8]	# Only pick out sideways normals, ignore depth normals.
		for x in range(len(normals)):
			if not normals[x-2] and not normals[x-1] and not normals[x]:
				free2move = True
				break
		if not free2move:
			# print('These two pieces would be stuck (splitting):')
			# print(p)
			# print('@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@')
			# print(shape)
			idx = ps.index(p)
			ps.remove(p)
			[ps.insert(idx+i,t) for i,t in enumerate(take_piece(p))]
			return True
	return False

def split_pieces(txtfile, partcnt):
	ts = asc.load_shapes(txtfile)
	if len(ts) != 1:
		raise ValueError('One shape expected in the file %s, but %i found.' % (txtfile,len(ts)))
	before_tricnt = asc.tricnt(ts[0])
	while len(ts) < partcnt:
		p = list(sorted(ts, key=lambda s:asc.tricnt(s), reverse=True))[0]
		ts.remove(p)
		ts += take_piece(p)
	if before_tricnt != sum((asc.tricnt(t) for t in ts)):
		print('\n~~~~~~~~~~~~~~~~~~\n'.join([str(t) for t in ts]))
		raise ValueError('||||||The number of triangles was %i, but is now %i' % (before_tricnt,sum((asc.tricnt(t) for t in ts))))
	while True:
		prelen = len(ts)
		# Make sure each piece is coherent in every layer.
		ps = []
		[ps.extend(cleanup_layers(t)) for t in ts]
		if before_tricnt != sum((asc.tricnt(t) for t in ps)):
			print('\n~~~~~~~~~~~~~~~~~~\n'.join([str(t) for t in ps]))
			raise ValueError('####The number of triangles was %i, but is now %i' % (before_tricnt,sum((asc.tricnt(t) for t in ps))))
		for p in ps:
			if open_cage(p,ps):
				break	# Need to clean up uncaged piece before continuing.
		if before_tricnt != sum((asc.tricnt(t) for t in ps)):
			print('\n~~~~~~~~~~~~~~~~~~\n'.join([str(t) for t in ps]))
			raise ValueError('++++The number of triangles was %i, but is now %i' % (before_tricnt,sum((asc.tricnt(t) for t in ps))))
		ts = ps
		if prelen == len(ps):	# Nothing happened during unboxing?
			break
	after_tricnt = sum((asc.tricnt(t) for t in ts))
	if before_tricnt != after_tricnt:
		raise ValueError('The number of triangles was %i, but is now %i' % (before_tricnt,after_tricnt))
	return ts

if __name__ == '__main__':
	ts = split_pieces('level', 2)
	print('\n~~~~~~~~~~~~~~~~~~\n'.join([asc.shape2str(t) for t in ts]))
