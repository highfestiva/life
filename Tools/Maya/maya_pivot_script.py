
# Copyright 2009 Righteous Games
# Created by Jonas Byström, 2009-12-13 for Maya/Righteous Engine tool chain.
# To be used inside of Maya for facilitating pivot moves.


import maya.cmds as cmds


def getroot(objs):
    o = objs[0]
    p = o
    while p:
        p = cmds.listRelatives(o, parent=True)
        if p:
            o = p
    return o


def setpivots(objs):
    root = getroot(objs)
    origin = cmds.xform(root, q=True, ws=True, t=True)
    for o in objs:
        cmds.xform(o, piv=origin, ws=True)
    return origin, len(objs)


objs = cmds.ls(selection=True)
if objs:
    pivot, num = setpivots(objs)
    print "Moved %i objects to %s." % (num, str(pivot))
else:
    print "Error: no objects selected!"
