#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import codecs
import tv3d
from asc import GRID
import piece2obj
from vec3 import vec3


G2 = GRID*2


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
	assert vec3(GRID,GRID,GRID) in o[0].gfxmesh.vertices
	assert len(o[0].gfxmesh.indices) == 12*3	# Reduced two in the back, two in the front (and of course none in the middle).
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysBox
	assert o[0].physgeoms[0].pos == vec3(0,0,0)
	assert o[0].physgeoms[0].size.x == G2
	assert o[0].physgeoms[0].size.y == G2
	assert o[0].physgeoms[0].size.z == G2

def testirregulargfx():
	o = piece2obj.piece2obj(['l'])
	assert len(o) == 1
	assert len(o[0].gfxmesh.vertices) == 10
	assert vec3(0,0,GRID) in o[0].gfxmesh.vertices
	assert len(o[0].gfxmesh.indices) == 14*3	# Front and back optimization.
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysMesh
	assert o[0].physgeoms[0].pos == vec3(0,0,0)

def testasymmeticcubes():
	length = 7
	wide,high,deep = (['X'*length],vec3(length,1,1)), (['X']*length,vec3(1,length,1)), ('\n---\n'.join(['X']*length).split(),vec3(1,1,length))
	def innercubetest(ascii,size):
		o = piece2obj.piece2obj(ascii)
		assert len(o) == 1
		assert len(o[0].gfxmesh.vertices) == 8
		assert size/2 in o[0].gfxmesh.vertices
		assert len(o[0].gfxmesh.indices) == 12*3
		assert len(o[0].physgeoms) == 1
		assert type(o[0].physgeoms[0]) == piece2obj.PhysBox
	[innercubetest(ascii,size) for ascii,size in [wide,high,deep]]

def test3dcuboid():
	o = piece2obj.piece2obj(['XX','XX','---','XX','XX','---','XX','XX'])
	assert len(o) == 1
	assert len(o[0].gfxmesh.vertices) == 8
	assert vec3(-G2,-G2,-1.5*G2) in o[0].gfxmesh.vertices
	assert len(o[0].gfxmesh.indices) == 12*3
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysBox

def testtinyrotated2dcube():
	o = piece2obj.piece2obj(['<>'])
	assert len(o[0].gfxmesh.vertices) == 8
	assert piece2obj.invrotq == o[0].gfxmesh.q
	assert vec3(0,0,0) == o[0].gfxmesh.pos
	assert vec3(0,GRID,GRID) in o[0].gfxmesh.vertices
	assert len(o[0].gfxmesh.indices) == 12*3
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysBox

def testrotated2dcube():
	o = piece2obj.piece2obj(['/\\','´`'])
	assert len(o) == 1
	assert len(o[0].gfxmesh.vertices) == 8
	assert len(o[0].gfxmesh.indices) == 12*3
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysBox

def testrotated3dcuboid():
	o = piece2obj.piece2obj(codecs.open('rotated_3d_cuboid.txt', encoding='utf-8'))
	assert len(o) == 1
	assert piece2obj.invrotq == o[0].gfxmesh.q
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
	assert vec3(0,GRID*2/3,GRID) in o[0].gfxmesh.vertices
	assert len(o[0].gfxmesh.indices) == 8*3
	assert len(o[0].physgeoms) == 1
	assert type(o[0].physgeoms[0]) == piece2obj.PhysMesh

def testbig3dprism():
	o = piece2obj.piece2obj(codecs.open('big_3d_prism.txt', encoding='utf-8'))
	assert len(o[0].gfxmesh.vertices) == 6
	assert vec3(-GRID,-G2,-1.5*G2) in o[0].gfxmesh.vertices
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
	assert len([1 for geom in o[0].physgeoms if type(geom) == piece2obj.PhysBox]) == 13
	assert len([1 for geom in o[0].physgeoms if type(geom) == piece2obj.PhysMesh]) == 2


from functools import partial
from math import pi
import tv3d
tv3d.open(camdist=80,camangle=(0,0,0),camrotspeed=(0,0,0.1),fov=8,addr='localhost:2541')
tv3d.joint_terminate = False
dodraw = True
def drawmesh(f,v,i):
	r = f(v,i)
	if dodraw:
		tv3d.releaseobjects()
		tv3d.creategfxobject([f for xyz in v for f in xyz],i)
		tv3d.sleep(0.05)
	return r
def physstuff(f, oshape, shape):
	global dodraw
	dodraw = False
	r = f(oshape,shape)
	dodraw = True
	return r
def drawwithphys(toobj,shape):
	tv3d.debug(True)
	obj = toobj(shape)
	tv3d.releaseobjects()
	tv3d.clearphys()
	for phys in obj.physgeoms:
		if type(phys) == piece2obj.PhysBox:
			tv3d.initphysbox(phys.q, phys.pos, phys.size)
		else:
			tv3d.initphysmesh(phys.q, phys.pos, phys.vertices, phys.indices)
	tv3d.initgfxmesh(obj.gfxmesh.q, obj.gfxmesh.pos, [f for xyz in obj.gfxmesh.vertices for f in xyz], obj.gfxmesh.indices)
	tv3d.createobject()
	tv3d.sleep(0.5)
	tv3d.debug(False)
	tv3d.clearphys()
	return obj
piece2obj.mesh_extendfaces = partial(drawmesh, piece2obj.mesh_extendfaces)
piece2obj.mesh_crushfaces = partial(drawmesh, piece2obj.mesh_crushfaces)
piece2obj.shape2physgeoms = partial(physstuff, piece2obj.shape2physgeoms)
piece2obj.shape2obj = partial(drawwithphys, piece2obj.shape2obj)
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
