#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import math
import mesh
import unittest
from vec3 import vec3

def _roundlist(l, n):
	return [round(float(x),n) for x in l]

def _normalize(ns):
	def xyzchunks(l):
		for i in range(0, len(l), 3):
			yield l[i:i+3]
	ns2 = [list(vec3(x,y,z).normalize()) for x,y,z in xyzchunks(ns)]
	return list(item for iter_ in ns2 for item in iter_)

class TestMesh(unittest.TestCase):

	def test_basic_angle_fast(self):
		af = lambda v,x,y,z: round(mesh._angle_fast(v.normalize(),x,y,z), 3)
		self.assertEqual(af(vec3(1,0,0),1,0,0), 0)
		self.assertEqual(af(vec3(0,1,0),0,1,0), 0)
		self.assertEqual(af(vec3(0,0,1),0,0,1), 0)
		self.assertEqual(af(vec3(1,0,0),0,1,0), 0.7)
		self.assertEqual(af(vec3(1,0,0),0,0,1), 0.7)
		self.assertEqual(af(vec3(0,1,0),0,0,1), 0.7)

	def test_advanced_angle_fast(self):
		af = lambda v,x,y,z: round(mesh._angle_fast(v.normalize(),x,y,z), 3)
		self.assertEqual(af(vec3(1,1,0),1,0,0), 0.785)
		self.assertEqual(af(vec3(1,1,0),0,1,0), 0.785)
		self.assertEqual(af(vec3(1,1,0),0,0,1), 0.7)
		self.assertEqual(af(vec3(1,-1,0),1,0,0), 0.785)
		self.assertEqual(af(vec3(-1,0,-5),0,0,+1), 0.7)
		self.assertEqual(af(vec3(-1,0,-2),0,0,+1), 0.7)
		self.assertEqual(af(vec3(-1,0,-1),0,0,+1), 0.7)
		self.assertEqual(af(vec3(-5,0,-1),0,0,+1), 0.7)

	def test_split_triangles(self):
		vs = [0,0,0, 0,2,0, 1,1,0, 0,1,-1]
		ts = [0,1,2, 0,1,3]
		ns = [0,0,1, 0,0,1, 0,0,1, -1,0,0, -1,0,0, -1,0,0]
		uvs = [0,0, 0,1, 1,1, 0,0, 0,1, 1,1]
		vs2, ts2, ns2, uvs2 = mesh.splitverts(vs, ts, ns, uvs)
		self.assertEqual(vs[0:3*3]+vs[-1*3:]+vs[0:2*3], vs2)
		self.assertEqual([0,1,2,4,5,3], ts2)
		self.assertEqual(ns[0:3*3]+ns[-1*3:]+ns[3*3:5*3], ns2)
		uvs, uvs2 = _roundlist(uvs, 5), _roundlist(uvs2, 5)
		self.assertEqual(uvs[0:3*2]+uvs[-1*2:]+uvs[3*2:5*2], uvs2)

	def test_join_triangles(self):
		vs = [0,0,0, 0,2,0, 1,1,0, 0,1,-1]
		ts = [0,1,2, 0,1,3]
		ns = _normalize([-1,0,1, -1,0,1, 0,0,1, -1,0,1, -1,0,1, -1,0,0])
		uvs = [0,0, 0,1, 1,1, 0,0, 0,1, 1,1]
		vs2, ts2, ns2, uvs2 = mesh.splitverts(vs, ts, ns, uvs)
		self.assertEqual(len(vs2), 4*3)
		self.assertEqual(len(ts2), 6)
		self.assertEqual(len(ns2), 4*3)
		self.assertEqual(len(uvs2), 4*2)
		self.assertEqual(vs, vs2)
		self.assertEqual(ts, ts2)
		ns, ns2 = _roundlist(ns, 6), _roundlist(ns2, 6)
		self.assertEqual(ns[0:3*3]+ns[-1*3:], ns2)
		self.assertEqual(uvs[0:3*2]+uvs[-1*2:], uvs2)

if __name__ == '__main__':
	unittest.main()
