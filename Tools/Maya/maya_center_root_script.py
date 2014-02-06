
# Copyright 2009-2014 Pixel Doctrine
# Created by Jonas Byström, 2014-02-05 for Maya/Righteous Engine tool chain.
# To be used inside of Maya for facilitating pivot moves.


import maya.cmds as cmds


def getroot(objs):
	o = objs[0]
	p = o
	while p:
		p = cmds.listRelatives(o, parent=True)
		if p and type(p) == list:
		    p = p[0]
		if p:
			o = p
	return o


def translate_faces(objs):
	root = getroot(objs)
	origin = cmds.xform(root, q=True, ws=True, t=True)
	faces = root+".f[:]"
	cmds.select(faces, replace=True)
	cmds.move(origin[0], origin[1], origin[2], relative=True)
	cmds.select(root, replace=True)
	cmds.move(-origin[0], -origin[1], -origin[2], relative=True)


objs = cmds.ls(selection=True)
if objs:
	num = translate_faces(objs)
	print "Moved faces to origin."
else:
	print "Error: no objects selected!"
