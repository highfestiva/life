#!/usr/bin/env python3
# Recursive mandelbrot fractal.

from trabant import *

resolution = 1/20
zcache = {}

async_load(True)
cam(distance=3)
bg('#cb3')

def f_range(lo, hi):
    while lo < hi+resolution/10:
        yield lo
        lo += resolution

square  = lambda x1,x2,y1,y2: create_box(((x1+x2)/2,0,(y1+y2)/2), side=(x2-x1), col='#333', mat='flat', static=True)

def mandel_hit(x, y):
    z = complex(x,y)
    if z not in zcache:
        c = z
        for n in range(8):
            if abs(z) > 2:
                zcache[z] = False
                break
            z = z*z + c
        else:
            zcache[z] = True
    return zcache[z]

def mandel_square(x1,x2, y1,y2):
    top    = [mandel_hit(x,y1) for x in f_range(x1,x2)]
    bottom = [mandel_hit(x,y2) for x in f_range(x1,x2)]
    left   = [mandel_hit(x1,y) for y in f_range(y1,y2)]
    right  = [mandel_hit(x2,y) for y in f_range(y1,y2)]
    assert len(top) == len(bottom) == len(left) == len(right)
    hits = sum(top)
    if hits in (0,len(top)) and hits == sum(bottom) == sum(left) == sum(right):
        if top and top[0]:
            square(x1,x2, y1,y2)
    else:
        if len(top) > 1:
            mandelbrot(x1,x2, y1,y2)

def mandelbrot(x1,x2, y1,y2):
    half = (x2-x1)/2
    mandel_square(x1,x1+half, y1,y1+half)
    mandel_square(x1+half,x2, y1,y1+half)
    mandel_square(x1,x1+half, y1+half,y2)
    mandel_square(x1+half,x2, y1+half,y2)

mandelbrot(-2,+1, -1.5,+1.5)

while loop():
    cam(angle=gametime())
