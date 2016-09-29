#!/usr/bin/env python3

from trabant import *

bg('#7af')
floor = create_box(pos=(0,0,-0.5), side=(60,6,1), static=True)

def sphere2bird(orientation, gfx, phys):
	# Extend a beak and a tail by extruding a couple of
	# coordinates in the middle of the sphere.
	gfx.vertices[len(gfx.vertices)//2-6] += vec3(0.4,0,-0.1)
	gfx.vertices[len(gfx.vertices)//2] += vec3(-0.7,0,0.2)
	return orientation, gfx, phys
bird = create_sphere(pos=(-20,0,5), col='#c00', process=sphere2bird, static=True)	# Static keeps the bird from falling.
cam(target=bird, distance=30, smooth=0.8)

def create_pig_box(pos, scale):
	create_box(pos+vec3(-1.5,0,1.5)*scale, side=vec3(0.5,2,3)*scale, mass=40*scale, col='#a62', mat='flat')
	create_box(pos+vec3(+1.5,0,1.5)*scale, side=vec3(0.5,2,3)*scale, mass=40*scale, col='#a62', mat='flat')
	create_box(pos+vec3(0,0,3.25)*scale, side=vec3(4,2,0.5)*scale, mass=40*scale, col='#a62', mat='flat')
	return create_sphere(pos+vec3(0,0,1)*scale, radius=scale, mass=70*scale, col='#4e4')	# Pig is a sphere.

create_pig_box(vec3(13,0,0), 2)
create_pig_box(vec3(13,0,7), 1)
create_pig_box(vec3(22,0,0), 2)
create_pig_box(vec3(22,0,7), 1.5)

campos = vec3(0,0,0)
while loop():
	cam(pos=campos)
	campos.x += 0.5 if campos.x < 10 else (40-campos.x)/20
	if timeout(3):
		break
while loop():
	cam(pos=campos)
	campos.x -= 0.5 if campos.x > 10 else campos.x/20
	if timeout(3):
		break
cam(pos=(0,0,0))

bird.physics_type(static=False)
bird.vel((25,0,6))

while loop():
	bird.avel(bird.avel()*0.9)
