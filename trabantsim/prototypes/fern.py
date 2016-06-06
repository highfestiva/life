#!/usr/bin/env python3

from trabant import *

branch_clones,leaf_clones = [],[]

fg(outline=False, shadows=False)
bg(col='#44d')

def gen_fern(pos, ang, d_ang, length, depth=1):
    if length < (0.1 if depth==1 else 0.02):
        return [],[]
    ori = quat().rotate_y(ang*pi/180)
    v = ori * vec3(0,0,length)
    trunk = [(pos+v*0.5, ori, length)]
    if depth == 1:  # trunk
        b1,l1 = gen_fern(pos+v*0.3, ang+80, -abs(d_ang)*1.5, length**1.2/3, depth+1)
        b2,l2 = gen_fern(pos+v*0.7, ang-80, +abs(d_ang)*1.5, length**1.2/3, depth+1)
        b3,l3 = gen_fern(pos+v, ang+d_ang, d_ang, length*0.85, depth)
        branches,leaves,sub_leaves = b1+b2+b3, l1+l2+l3, []
    else:   # branch
        length2 = length*0.7
        q = quat().rotate_y((ang+90)*pi/180)
        u = q * vec3(0,0,length2)
        leaves  = [(pos+v*0.8+u, q, length2)]
        leaves += [(pos+v*0.8-u, q, length2)]
        branches,sub_leaves = gen_fern(pos+v, ang+d_ang, d_ang, length*0.9, depth)
    return trunk+branches, leaves+sub_leaves

def clone_capsules(posdata, radius_factor):
    '''It's faster to clone objects than to create new instances.'''
    min_capsule_resolution = lambda r,l:(2,2)
    maxlen = max(posdata, key=lambda l:l[2])[2]
    while posdata:
        maxlen *= 0.9
        clones = sorted([l for l in posdata if l[2]>=maxlen], key=lambda l:l[2])
        if clones:
            i = len(clones)//2
            pos,ori,length = clones[i]
            leaf = create_capsule(pos, ori, radius=length*radius_factor, length=length, col='#5d5', mat='flat', static=True, resolution=min_capsule_resolution)
            create_clones(leaf, [(pos,ori) for pos,ori,length in clones[:i]+clones[i+1:]], static=True)
            for c in clones:
                posdata.remove(c)

branches, leaves = gen_fern(vec3(0,0,-4), -20, +2, 1.3)
clone_capsules(branches, 1/10)
clone_capsules(leaves, 1/2)

while loop():
    cam(time.time()%(2*pi))
