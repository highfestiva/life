#!/usr/bin/env python3


from trabant.objects import *
from trabant.math import quat,vec3


def createmesh(vertices,triangles):
	return GfxMesh(quat(),vec3(),vertices,triangles),[PhysMesh(quat(),vec3(),vertices,triangles)]

def createcube(side):
	return GfxMesh(quat(),vec3(),[],[]),[]

def createsphere(radius):
	return GfxMesh(quat(),vec3(),[],[]),[]
