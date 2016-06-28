#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Terminal Velocity prototype, accelerometer controlled.

from trabant import *

def play(level, ship):

	ship.pos((0,-300,0))
	ship.vel((50,0,0))
	mover = ship.create_engine(push_abs_engine, max_velocity=50, friction=50)
	#roller = ship.create_engine(push_turn_rel_engine, friction=0.5)    # Handles both banking and levelling ship.

	terrain_meshes,patch_size = {},120

	bg(col='#aaa')
	fog(350,430)

	def create_terrain_patch(px,py):
		x,y,grid = px*patch_size,py*patch_size,6
		g1,g2,s = grid+1,grid//2,patch_size//grid
		vertices,triangles = [],[]
		for v in range(0,grid+1):
			for u in range(0,grid+1):
				lpos = vec3((u-g2)*s, 0, (v-g2)*s)
				wpos = vec3(x,0,y)+lpos
				lpos.y = -84 + 19*sin2(wpos.x/73) + 13*sin2(wpos.y/123) + 17*sin2((wpos.x+wpos.y)/173) + 28*(sin2(wpos.x/243)+sin2(wpos.y/253))
				vertices.append(lpos)
				if v < grid and u < grid:
					triangles += [v*g1+u, v*g1+u+1, (v+1)*g1+u, v*g1+u+1, (v+1)*g1+u+1, (v+1)*g1+u]
		col = int(-cos(py/19)*80+160)*256 + int(sin(px/17)*80+160)*65536    # Vary the patch color a little.
		return create_mesh(vertices, triangles, pos=(x,0,y), mat='noise', col='#%6.6x'%col, static=True)

	def update_terrain(pos):
		'''Add new visible patches, drop no longer visible ones.'''
		nonlocal terrain_meshes
		old_meshes,created = dict(terrain_meshes),False
		top,left,s = int(pos.z)//patch_size-3,int(pos.x)//patch_size-3,7
		for y in reversed(range(top,top+s)):
			for x in range(left,left+s):
				key = y*3301919+x    # Anything large prime ok for hashing.
				if key in terrain_meshes:
					del old_meshes[key]
				elif not created:
					terrain_meshes[key],created = create_terrain_patch(x,y),True
		# Drop patches that have gone out of range.
		for key,mesh in old_meshes.items():
			del terrain_meshes[key]
			mesh.release()

	camsmooth = 0
	while loop(end_after=20):
		if gametime() < 4:
			camsmooth = lerp(camsmooth, 0.8, 0.1)
			cam(smooth=camsmooth)

		position,orientation = ship.pos(),ship.orientation()

		update_terrain(position + orientation*vec3(100,0,0))

		# Move ship.
		#yaw_force = acc.roll*6 - sum(t.x*10-5 for t in taps()) - 5*keydir().x    # Control by either accelerometer, tapping or keyboard.
		gnd_pos = pick_objects(position + orientation*vec3(50,-200,0), vec3(0,1,0))
		updown = 0
		if gnd_pos:
			_,pos = gnd_pos[0]
			keep_distance = 20
			updown = (pos.y - keep_distance - position.y) / 5
			updown = max(min(updown,5),-7)
		updown -= ship.vel().y/20
		mover.force((0.9, updown, -10*keydir().x))
		ship.orientation(lerp(quat(), quat().rotate_z(updown/7), 0.05).normalize())

		# Banking and nose.
		#roll_force = -yaw_force*1.3 + (orientation*vec3(1,0,0)).z*10    # Banking.
		#pitch_factor = (orientation*vec3(0,0,1)).z*10 + ship.pos().z/10    # Level ship at around Z=0.
		#pitch_force = acc.pitch*6 - sum(t.y*20-10 for t in taps()) - 3*keydir().y - pitch_factor    # Control nose by either accelerometer, tapping or keyboard.
		#roller.force((pitch_force,0,roll_force))
	return ship


def transition(ship):
	cam(smooth=0.95)
	speed,d = 50,30
	while loop(end_after=1):
		d = lerp(d,150,0.05)
		cam(distance=d)
		speed = lerp(speed,400,0.05)
		up = 3
		ship.vel((speed,-up*up,0))
	bgcol = vec3(0.6,0.6,0.6)
	while loop(end_after=1):
		speed = 400
		up = lerp(up,8.5,0.1)
		ship.vel((speed,-3*up*up,0))
		ship.orientation(quat().rotate_z(-up/11))
		bgcol = lerp(bgcol,vec3(),up/70)
		bg(bgcol)
	release_objects(keep=lambda obj: obj in ship.ship_parts)
	sleep(0.2)
	cam(distance=100, angle=(0,0,0), target=0, smooth=0)
	ship.vel((0,0,0))
	ship.pos((0,0,0))
	bg(col='#000')
