#!/usr/bin/env python3

from trabant import *
from trabant.gameapi import setvar

bg('#7af')
friction(0.8, bounce=0.2)
# Due to physics engine instability we set the collision sound scale,
# or every little quiver will be played as a collision sound.
setvar('Ui.Sound.CollisionMassScale', 300.0)

def process_sphere2bird(orientation, gfx, phys):
	# Extend a beak and a tail by extruding a couple of
	# coordinates in the middle of the sphere.
	gfx.vertices[len(gfx.vertices)//2-6] += vec3(0.4,0,-0.1)
	gfx.vertices[len(gfx.vertices)//2] += vec3(-0.7,0,0.2)
	return orientation, gfx, phys

def process_sphere2pig(orientation, gfx, phys):
	# Extend a snout by extruding a couple of coordinates in the
	# top of the sphere and rotating it to front.
	r = gfx.vertices[0].z
	for i in range(24):
		gfx.vertices[i] = (gfx.vertices[i]*0.8).with_z(r*1.1)
	for i in range(24,36):
		gfx.vertices[i] = (gfx.vertices[i]*0.4).with_z(r*0.8)
	orientation = orientation.rotate_x(pi/2)
	return orientation, gfx, phys

def create_pig_box(pos, scale):
	create_box(pos+vec3(-1.5,0,1.5)*scale, side=vec3(0.5,2,3)*scale, mass=40*scale, col='#642', mat='flat')
	create_box(pos+vec3(+1.5,0,1.5)*scale, side=vec3(0.5,2,3)*scale, mass=40*scale, col='#642', mat='flat')
	create_box(pos+vec3(0,0,3.25)*scale, side=vec3(4,2,0.5)*scale, mass=40*scale, col='#642', mat='flat')
	pig = create_sphere(pos+vec3(0,0,1)*scale, radius=scale, mass=20*scale, col='#1c1', mat='flat', process=process_sphere2pig)
	pig.health = 2000*scale
	pig.scale = scale
	return pig

while True:
	release_objects()

	floor = create_box(pos=(0,0,-0.5), side=(55,12,1), static=True)

	bird = create_sphere(pos=(-22,0,1), col='#c00', process=process_sphere2bird)
	cam(target=bird, pos=(40,0,0), distance=30, smooth=0.8)
	slingshot = create_capsule(pos=(-19,-2,3), col='#420', length=6, radius=0.5, static=True)

	pigs = [create_pig_box(vec3(13,0,0), 2),
			create_pig_box(vec3(13,0,7), 1),
			create_pig_box(vec3(22,0,0), 2),
			create_pig_box(vec3(22,0,7), 1.5)]

	campos = vec3(40,0,0)
	# Pan camera from right (pigs) to left (bird).
	while loop(end_after=2):
		cam(pos=campos)
		campos.x -= 0.5 if campos.x > 10 else campos.x/20

	taps() # Flush any pending taps.
	guide = []

	while loop() and pigs:
		rubberband_attachment = slingshot.pos().with_y(0) + vec3(0,0,3)
		for tap in taps():
			drag = (rubberband_attachment - tap.pos3d()).with_y(0) * 0.5
			drag = drag if drag.length() <= 4 else drag.normalize(4)
			bird_pos = rubberband_attachment - drag
			bird.pos(bird_pos, orientation=quat())
			bird.vel((0,0,0))
			if not guide:
				guide = [create_sphere(radius=0.2, col=(1,1,1,1-i/8), static=True, trigger=True) for i in range(8)]
			for i,g in enumerate(guide):
				g.pos(bird_pos + drag*(i*0.7+0.5) + vec3(0,0,-0.05*i*i))
			if not tap.ispress:
				[g.release() for g in guide]
				guide = []
				bird.vel(drag*7)

		# Slow down rotation of bird and pigs.
		for obj in [bird]+pigs:
			obj.avel(obj.avel()*0.9)

		for pig,_,force,_ in collisions():
			if pig in pigs:
				force *= 1/pig.scale
				if -300 < force.z < 1000: # Only care about pigs being smashed.
					continue
				pig.health -= force.length()
				if pig.health <= 0:
					explode(pig.pos(), strength=0.3)
					pig.release()
					pigs.remove(pig)

	# Let victory sink in.
	while loop(end_after=2):
		pass
