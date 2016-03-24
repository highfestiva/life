#!/usr/bin/env python3

from trabant import *
from time import time

asc = '''
/\ X
`XXX'''

cam(distance=300)

def perftest(sf):
    f = eval(sf)
    t = time()
    objs = []
    for z in range(6,1,-1):
        for x in range(-10,10):
            objs.append(f((x*5,0,z*20)))
    for o in objs:
        o.release()
    print('%s\t%g' % (sf.split(': ')[1], time()-t))

def testall():
    perftest('lambda pos: create_ascii_object(asc, pos, vel=(0,0,-5))')
    perftest('lambda pos: create_ascii_object(asc, pos, static=True)')
    perftest('lambda pos: create_ascii_object(asc, pos, vel=(0,0,-5), trigger=True)')
    perftest('lambda pos: create_sphere(pos, vel=(0,0,-5))')
    perftest('lambda pos: create_sphere(pos, static=True)')
    perftest('lambda pos: create_sphere(pos, vel=(0,0,-5), trigger=True)')
    perftest('lambda pos: create_box(pos, vel=(0,0,-5))')
    perftest('lambda pos: create_box(pos, static=True)')
    perftest('lambda pos: create_box(pos, vel=(0,0,-5), trigger=True)')

async_load(True)
testall()
