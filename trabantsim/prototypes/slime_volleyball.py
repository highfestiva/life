#!/usr/bin/env python3
# Slime Volleyball prototype.

# The slimes (playing volleyball) in this prototype are based on physical spheres, but the graphics
# are rendered as half-spheres. To compensate for the invisible bottom part of each sphere, the ground
# is raised by the radius of the spheres.

from trabant import *
from trabant.gameapi import haltsim
from trabant.objects import gfxoffset

trabant_init(fps=60) # Raise framerate to get a smoother experience.
bg('#079')
cam(distance=80, pos=(0,0,5), fov=10) # Long distance, small FoV for an almost isometric view.
g = 20
gravity((0,0,-g))

# Setup ground and net. Ground graphic is rendered 1 meter above physical ground to compensate for half-sphere slimes.
ground = create_box((0,0,-1-15), side=(30,6,15*2), mat='flat', col='#420', static=True, process=gfxoffset(0,0,1))
net = create_box((0,0,0), side=(0.1,1,2), mat='flat', col='#999', static=True)

def create_slime(pos, col):
	# We create the slimes as physical spheres, but we flatten the bottom of graphical spheres.
	def flat_bottom(orientation,gfx,phys):
		gfx.vertices = [v.with_z(v.z if v.z>0 else 0) for v in gfx.vertices]
		return orientation,gfx,phys
	slime = create_sphere(pos, col=col, process=flat_bottom)
	slime.create_engine(stabilize) # Make sure slimes don't start rotating.
	slime.create_engine(vel_abs_xy_engine) # Use engine so we don't cause up/down/z jitter.
	eye = create_sphere(pos+vec3(-0.06*pos.x,-0.6,0.5), radius=0.25, col='#fff', trigger=True)
	pupil = create_sphere(pos+vec3(-0.07*pos.x,-0.8,0.55), radius=0.1, col='#000', trigger=True)
	slime.joint(fixed_joint, eye)
	slime.joint(fixed_joint, pupil)
	return slime
player,computer = [create_slime(vec3(x,0,0), col=c) for x,c in ([-5,'#ff0'],[+5,'#f00'])]

# Create a white volley ball.
ball = create_sphere((-5,0,3), radius=0.4, mass=1, col='#fff')

def steer(avatar, movex, jump, minx, maxx):
	'''Allows control of slime within its area and jumping when on ground.'''
	p = avatar.pos()
	movex = 0 if (p.x >= maxx and movex > 0) or (p.x <= minx and movex < 0) else movex
	jump = jump if jump > 0 and p.z < 0.1 else 0
	avatar.engines[-1].force((movex*0.3, 0, jump*0.6))
	if p.x > maxx+0.01:
		avatar.pos(p.with_x(maxx))
	elif p.x < minx-0.01:
		avatar.pos(p.with_x(minx))

def ai(computer, ball):
	'''Moves to where the ball will hit, if on computer side of the net. Jumps just before impact.'''
	p,v,head_hight = ball.pos(),ball.vel(),0.8+0.4
	h_peak = v.z*v.z / (2*g)
	h_peak_relative_head = h_peak + p.z - head_hight
	h_peak_relative_head = h_peak_relative_head if h_peak_relative_head >= 0 else 0
	t_peak2now = sqrt(2*h_peak/g) * (-1 if v.z>=0 else +1)
	t_peak2head = sqrt(2*h_peak_relative_head/g)
	t = t_peak2head - t_peak2now
	x = p.x + v.x * t + 0.2
	if x > 0: # On our side of the net?
		movex = max(-1,min(+1,x-computer.pos().x))
		jump = 1 if t < 0.2 else 0
		steer(computer, movex, jump, +1.1, +10)
	else:
		steer(computer, 0, 0, +1.1, +10)

while loop():
	steer(player, keydir().x, keydir().y, -10, -1.1)
	ai(computer, ball)

	# Every time the ball bounces we make sure it gets full velocity.
	for obj,other,_,_ in collisions():
		if obj == ball:
			ball.vel((ball.vel().normalize(10) + other.vel()*0.3).with_y(0))
	# Bounce the ball on the invisible back side of the court.
	ball.bounce_in_rect((-11,-0.05,-1),(+11,+0.05,+30))

	# Check if either side won (i.e. when the ball touches ground).
	if ball.pos().z < 0.4:
		# Pause simulation and let user see ball hitting ground.
		haltsim(True)
		userinfo('Inferior human controlled slime wins.' if ball.pos().x>0 else 'Angry red slimonds wins.')
		while timein(1) and loop():
			pass
		userinfo('')
		# Reset positions.
		[avatar.engines[-1].force((0,0,0)) for avatar in (player,computer)]
		def reset(obj, p):
			obj.pos(p)
			obj.vel((0,0,0), avel=(0,0,0))
		# Winner serves.
		ballpos = (+5,0,3) if ball.pos().x < 0 else (-5,0,3)
		[reset(obj,pos) for obj,pos in [(player,(-5,0,0)), (computer,(+5,0,0)), (ball,ballpos)]]
		keys() # Flush keys.
		haltsim(False)
