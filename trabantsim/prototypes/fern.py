#!/usr/bin/env python3

from trabant import *
from trabant.objects import nophys

fg(outline=False, shadows=False)
bg(col='#44d')

def generate_fern(pos, ang, d_ang, length, depth=1):
    if length < (0.1 if depth==1 else 0.02):    # Branches grow thinner.
        return [],[]
    ori = quat().rotate_y(ang*pi/180).rotate_x(abs(ang)*0.3*pi/180) # X-rotation bends branches downwards.
    v = ori * vec3(0,0,length)
    trunk = [(pos+v*0.5, ori, length)]
    if depth == 1:  # trunk
        b1,l1 = generate_fern(pos+v*0.3, ang+80, -abs(d_ang)*1.5, length**1.2/3, depth+1)
        b2,l2 = generate_fern(pos+v*0.7, ang-80, +abs(d_ang)*1.5, length**1.2/3, depth+1)
        b3,l3 = generate_fern(pos+v, ang+d_ang, d_ang, length*0.85, depth)
        branches,leaves,sub_leaves = b1+b2+b3, l1+l2+l3, []
    else:   # branch
        length2 = length*0.7
        q = quat().rotate_y((ang+90)*pi/180).rotate_x(ang*0.2*pi/180)   # X-rotation tilt leaves upwards.
        u = q * vec3(0,0,length2)
        leaves  = [(pos+v*0.8+u, q, length2)]
        leaves += [(pos+v*0.8-u, q, length2)]
        branches,sub_leaves = generate_fern(pos+v, ang+d_ang, d_ang, length*0.9, depth)
    return trunk+branches, leaves+sub_leaves

def create_capsules(posdata, radius_factor):
    min_capsule_resolution = lambda r,l:(2,2)
    maxlen = max(posdata, key=lambda l:l[2])[2]
    while posdata:
        maxlen *= 0.9
        clones = [l for l in posdata if l[2]>=maxlen]
        if clones:
            pos,ori,_ = clones[0]
            length = sum([length for _,_,length in clones]) / len(clones)
            original = create_capsule(pos, ori, radius=length*radius_factor, length=length, col='#5d5', mat='flat', resolution=min_capsule_resolution, process=nophys)
            create_clones(original, [(pos,ori) for pos,ori,_ in clones[1:]], static=True)
            for c in clones:
                posdata.remove(c)

# It's faster to create cloned objects than unique ones. Therefore we
# generate positional information first, then create the objects.
branches, leaves = generate_fern(vec3(0,0,-4), -20, +2, 1.3)
create_capsules(branches, radius_factor=1/10)
create_capsules(leaves, radius_factor=1/2)

while loop():
    cam(gametime())
