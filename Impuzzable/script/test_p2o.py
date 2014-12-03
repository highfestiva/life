#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import codecs
import tv3d
from asc import GRID
import piece2obj
from vec3 import vec3


def testcrds():
	assert piece2obj.crd2diamondcrd(vec3(1,2,0)) == vec3(0,0,0)
	assert piece2obj.crd2diamondcrd(vec3(2,0,0)) == vec3(1,0,0)
	assert piece2obj.crd2diamondcrd(vec3(3,2,0)) == vec3(1,1,0)
	assert piece2obj.crd2diamondcrd(vec3(4,5,0)) == vec3(0,4,0)
	assert piece2obj.crd2diamondcrd(vec3(5,5,0)) == vec3(1,4,0)
	assert piece2obj.crd2diamondcrd(vec3(2,1,2)) == vec3(1,1,2)
	assert piece2obj.crd2diamondcrd(vec3(3,2,2)) == vec3(1,1,2)
	assert piece2obj.diamondcrd2crds(vec3(1,4,0)) == [vec3(5,5,0),vec3(6,4,0)]
	assert piece2obj.diamondcrd2crds(vec3(0,4,0)) == [vec3(4,5,0),vec3(5,6,0)]
	assert piece2obj.diamondcrd2crds(vec3(1,0,2)) == [vec3(1,1,2),vec3(2,0,2)]
	assert piece2obj.diamondcrd2crds(vec3(1,1,2)) == [vec3(2,1,2),vec3(3,2,2)]

def test2dcube():
	o = piece2obj.piece2obj(['X'])
	assert len(o) == 1
	assert len(o[0].gfxmesh.vertices) == 8
	assert vec3(1,1,1) in o[0].gfxmesh.vertices
	assert len(o[0].gfxmesh.indices) == 12*3	# Reduced two in the back, two in the front (and of course none in the middle).
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysBox
	assert o[0].physgeoms[0].size.x == GRID*2
	assert o[0].physgeoms[0].size.y == GRID*2

def testirregulargfx():
	o = piece2obj.piece2obj(['l'])
	assert len(o) == 1
	assert len(o[0].gfxmesh.vertices) == 10
	assert vec3(0.5,0.5,1) in o[0].gfxmesh.vertices
	assert len(o[0].gfxmesh.indices) == 14*3	# Front and back optimization.
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysMesh

def testasymmeticcubes():
	length = 7
	wide,high,deep = (['X'*length],vec3(length,1,1)), (['X']*length,vec3(1,length,1)), ('\n---\n'.join(['X']*length).split(),vec3(1,1,length))
	def innercubetest(ascii,size):
		o = piece2obj.piece2obj(ascii)
		assert len(o) == 1
		assert len(o[0].gfxmesh.vertices) == 8
		assert size in o[0].gfxmesh.vertices
		assert len(o[0].gfxmesh.indices) == 12*3
		assert len(o[0].physgeoms) == 1
		assert type(o[0].physgeoms[0]) == piece2obj.PhysBox
	[innercubetest(ascii,size) for ascii,size in [wide,high,deep]]

def test3dcuboid():
	o = piece2obj.piece2obj(['XX','XX','---','XX','XX','---','XX','XX'])
	assert len(o) == 1
	assert len(o[0].gfxmesh.vertices) == 8
	assert vec3(0,0,0) in o[0].gfxmesh.vertices
	assert len(o[0].gfxmesh.indices) == 12*3
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysBox

def testtinyrotated2dcube():
	o = piece2obj.piece2obj(['<>'])
	assert len(o[0].gfxmesh.vertices) == 8
	assert vec3(0.5,0.5,1) in o[0].gfxmesh.vertices
	assert len(o[0].gfxmesh.indices) == 12*3
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysBox

def testrotated2dcube():
	o = piece2obj.piece2obj(['/,','´`'])
	assert len(o) == 1
	assert len(o[0].gfxmesh.vertices) == 8
	assert len(o[0].gfxmesh.indices) == 12*3
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysBox

def testrotated3dcuboid():
	o = piece2obj.piece2obj(codecs.open('rotated_3d_cuboid.txt', encoding='utf-8'))
	assert len(o) == 1
	assert len(o[0].gfxmesh.vertices) == 8
	assert len(o[0].gfxmesh.indices) == 12*3
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysBox

def testrotatedoffset3dcuboid():
	o = piece2obj.piece2obj(codecs.open('rotated_offset_3d_cuboid.txt', encoding='utf-8'))
	assert len(o) == 1
	assert len(o[0].gfxmesh.vertices) == 8
	assert len(o[0].gfxmesh.indices) == 12*3
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysBox

def testsmall2dtriangle():
	o = piece2obj.piece2obj(['v'])
	assert len(o[0].gfxmesh.vertices) == 6
	assert vec3(0.5,0.5,1) in o[0].gfxmesh.vertices
	assert len(o[0].gfxmesh.indices) == 8*3
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysMesh

def testbig3dprism():
	o = piece2obj.piece2obj(codecs.open('big_3d_prism.txt', encoding='utf-8'))
	assert len(o[0].gfxmesh.vertices) == 6
	assert vec3(0,0,0) in o[0].gfxmesh.vertices
	assert len(o[0].gfxmesh.indices) == 8*3
	assert len(o[0].physgeoms) == 5
	assert type(o[0].physgeoms[0]) == piece2obj.PhysBox
	assert type(o[0].physgeoms[1]) == piece2obj.PhysBox
	assert type(o[0].physgeoms[2]) == piece2obj.PhysBox
	assert type(o[0].physgeoms[3]) == piece2obj.PhysMesh
	assert type(o[0].physgeoms[4]) == piece2obj.PhysMesh

def testdoublechunk():
	o = piece2obj.piece2obj(codecs.open('double_chunk.txt', encoding='utf-8'))
	assert len(o[0].physgeoms) == 4
	assert all([type(geom) == piece2obj.PhysBox for geom in o[0].physgeoms])

def testsimpleshape():
	o = piece2obj.piece2obj(codecs.open('simple.txt', encoding='utf-8'))
	assert len(o[0].gfxmesh.vertices) == 34
	assert len(o[0].gfxmesh.indices) == 56*3
	assert len(o[0].physgeoms) == 5
	assert all([type(geom) == piece2obj.PhysBox for geom in o[0].physgeoms])
	assert all([geom.q[0] == 1 for geom in o[0].physgeoms])

def testcomplexshape():
	o = piece2obj.piece2obj(codecs.open('complex.txt', encoding='utf-8'))
	assert len(o[0].gfxmesh.vertices) == 63
	assert len(o[0].gfxmesh.indices) == 104*3
	assert len(o[0].physgeoms) == 10


# from functools import partial
# from math import pi
# import tv3d
# tv3d.open(camdist=80,camangle=(pi/2,0,0),camrotspeed=(0,0.4,0),fov=15,addr='localhost:2541')
# tv3d.joint_terminate = False
# def draw(f,v,i):
	# r = f(v,i)
	# tv3d.releaseobjects()
	# tv3d.createmeshobject([f for xyz in v for f in xyz],i)
	# tv3d.sleep(0.3)
	# return r
# piece2obj.mesh_extendfaces = partial(draw, piece2obj.mesh_extendfaces)
# piece2obj.mesh_crushfaces = partial(draw, piece2obj.mesh_crushfaces)
testcrds()
test2dcube()
testirregulargfx()
testasymmeticcubes()
test3dcuboid()
testtinyrotated2dcube()
testrotated2dcube()
testrotated3dcuboid()
testsmall2dtriangle()
testbig3dprism()
testdoublechunk()
testsimpleshape()
testcomplexshape()
print('OK')
