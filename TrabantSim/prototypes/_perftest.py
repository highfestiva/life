#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#from trabant import *
from trabant.asc2obj import str2obj

#fg(outline=False)
#gravity((0,0,-0.1))

body = '\n---\n'.join([r'/XXXXXXXXXXXX\ ']   +
                      [r'XXXXXXXXXXXXXX ']*8 +
                      [r'/XXXXXXXXXXXX\ '])
#body = '/XXXXX\n---\nXXXXXX'
#body = [create_ascii_object(part,pos=pos,col=col) for part,pos,col in [(body,(0,0,-2),'#a23')]][0]
## gfx,phys = body.gfx,body.phys
## cam(target=body, angle=(-0.5,0,-pi/2))
## from trabant.gameapi import setvar
## setvar('Ui.3D.CamRotateZ', 0.005)
## setvar('Ui.3D.WireframeMode', True)
gfx,phys = str2obj(body,fast=False)
## print(len(gfx.vertices),len(gfx.indices))
## while loop(end_after=10.0):
	## body.pos()
