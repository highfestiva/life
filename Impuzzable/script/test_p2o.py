#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import codecs
import tv3d
from asc import GRID
import piece2obj
from vec3 import vec3


def test2dcube():
	o = piece2obj.piece2obj(['X'])
	assert len(o) == 1
	assert len(o[0].vertices) == 8
	assert vec3(1,1,1) in o[0].vertices
	assert len(o[0].indices) == 12*3	# Reduced two in the back, two in the front (and of course none in the middle).

def testirregulargfx():
	o = piece2obj.piece2obj(['l'])
	assert len(o) == 1
	assert len(o[0].vertices) == 10
	assert vec3(0.5,0.5,1) in o[0].vertices
	assert len(o[0].indices) == 14*3	# Front and back optimization.

def testassymeticcubes():
	length = 5
	wide,high,deep = (['X'*length],vec3(length,1,1)), (['X']*length,vec3(1,length,1)), ('\n---\n'.join(['X']*length).split(),vec3(1,1,length))
	def innercubetest(ascii,size):
		o = piece2obj.piece2obj(ascii)
		assert len(o) == 1
		assert len(o[0].vertices) == 8
		assert size in o[0].vertices
		assert len(o[0].indices) == 12*3
	[innercubetest(ascii,size) for ascii,size in [wide,high,deep]]

def test3dcuboid():
	o = piece2obj.piece2obj(['XX','XX','---','XX','XX','---','XX','XX'])
	assert len(o) == 1
	assert len(o[0].vertices) == 8
	assert vec3(0,0,0) in o[0].vertices
	assert len(o[0].indices) == 12*3

def testtinyrotated2dcube():
	o = piece2obj.piece2obj(['<>'])
	assert len(o[0].vertices) == 8
	assert vec3(0.5,0.5,1) in o[0].vertices
	assert len(o[0].indices) == 12*3

def testrotated2dcube():
	o = piece2obj.piece2obj(['/,','´`'])
	assert len(o) == 1
	assert len(o[0].vertices) == 8
	assert len(o[0].indices) == 12*3

def testrotated3dcuboid():
	o = piece2obj.piece2obj(codecs.open('rotated_3d_cuboid.txt', encoding='utf-8'))
	assert len(o) == 1
	assert len(o[0].vertices) == 8
	assert len(o[0].indices) == 12*3

def testrotatedoffset3dcuboid():
	o = piece2obj.piece2obj(codecs.open('rotated_offset_3d_cuboid.txt', encoding='utf-8'))
	assert len(o) == 1
	assert len(o[0].vertices) == 8
	assert len(o[0].indices) == 12*3

def testsmall2dtriangle():
	o = piece2obj.piece2obj(['v'])
	assert len(o[0].vertices) == 6
	assert vec3(0.5,0.5,1) in o[0].vertices
	assert len(o[0].indices) == 8*3

def testbig3dprism():
	o = piece2obj.piece2obj(codecs.open('big_3d_prism.txt', encoding='utf-8'))
	assert len(o[0].vertices) == 6
	assert vec3(0,0,0) in o[0].vertices
	assert len(o[0].indices) == 8*3

def testsimpleshape():
	o = piece2obj.piece2obj(codecs.open('simple.txt', encoding='utf-8'))
	assert len(o[0].vertices) == 34
	assert len(o[0].indices) == 56*3

def testcomplexshape():
	o = piece2obj.piece2obj(codecs.open('complex.txt', encoding='utf-8'))
	assert len(o[0].vertices) == 63
	assert len(o[0].indices) == 104*3


# from functools import partial
# from math import pi
# import tv3d
# tv3d.open(camdist=80,camangle=(pi/2,0,0),camrotspeed=(0,0.4,0),fov=15,addr='localhost:2541')
# def draw(f,v,i):
	# r = f(v,i)
	# tv3d.releaseobjects()
	# tv3d.createmeshobject([f for xyz in v for f in xyz],i)
	# tv3d.sleep(0.05)
	# return r
# piece2obj.mesh_extendfaces = partial(draw, piece2obj.mesh_extendfaces)
# piece2obj.mesh_crushfaces = partial(draw, piece2obj.mesh_crushfaces)
test2dcube()
testirregulargfx()
testassymeticcubes()
test3dcuboid()
testtinyrotated2dcube()
testrotated2dcube()
testrotated3dcuboid()
testsmall2dtriangle()
testbig3dprism()
testsimpleshape()
testcomplexshape()
print('OK')
