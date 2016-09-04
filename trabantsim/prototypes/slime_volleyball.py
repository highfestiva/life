#!/usr/bin/env python3
# Slime Volleyball prototype.

from trabant import *
from trabant.gameapi import haltsim
from trabant.objects import gfxoffset

trabant_init(fps=60)
bg('#058')
cam(distance=80, pos=(0,0,5), fov=10)
gravity((0,0,-20))

def flat_bottom(orientation,gfx,phys):
	gfx.vertices = [v.with_z(v.z if v.z>0 else 0) for v in gfx.vertices]
	return orientation,gfx,phys

floor = create_box((0,0,-1-15), side=(30,4,15*2), mat='flat', col='#420', static=True, process=gfxoffset(0,0,1))
net = create_box((0,0,0), side=(0.1,1,2), mat='flat', col='#999', static=True)
player,computer = [create_sphere((x,0,0), col=c, process=flat_bottom) for x,c in ([-5,'#ff0'],[+5,'#f00'])]
player.create_engine(stabilize)
computer.create_engine(stabilize)
player.create_engine(vel_abs_xy_engine)
computer.create_engine(vel_abs_xy_engine)
ball = create_sphere((-5,0,3), radius=0.4, mass=1, col='#fff')

def steer(avatar, movex, jump, minx, maxx):
	p = avatar.pos()
	movex = 0 if (p.x >= maxx and movex > 0) or (p.x <= minx and movex < 0) else movex
	jump = jump if jump > 0 and p.z < 0.1 else 0
	avatar.engines[-1].force((movex*0.6, 0, jump*0.6))
	if p.x > maxx+0.01:
		avatar.pos(p.with_x(maxx))
	elif p.x < minx-0.01:
		avatar.pos(p.with_x(minx))

while loop():
	steer(player, keydir().x, keydir().y, -10, -1.1)
	steer(computer, 0, 0, +1.1, +10)

	# Every time the ball bounces we make sure it gets full velocity.
	for obj,other,_,_ in collisions():
		if obj == ball:
			ball.vel((ball.vel().normalize(10) + other.vel()*0.3).with_y(0))
	# Bounce the ball on the invisible back side of the court.
	ball.bounce_in_rect((-11,-0.1,-1),(+11,+0.1,+30))

	# If ball hit ground we pause for a second, the let the winner serve.
	if ball.pos().z < 0.4:
		# Pause simulation and let user see ball hitting ground.
		haltsim(True)
		while timein(1) and loop():
			pass
		# Reset positions and stop all objects.
		player.pos((-5,0,0))
		player.engines[-1].force((0,0,0))
		player.vel((0,0,0), avel=(0,0,0))
		computer.pos((+5,0,0))
		computer.engines[-1].force((0,0,0))
		computer.vel((0,0,0), avel=(0,0,0))
		ball.vel((0,0,0), avel=(0,0,0))
		keys()
		# Winner serves.
		ball.pos((+5 if ball.pos().x < 0 else -5,0,3))
		haltsim(False)
