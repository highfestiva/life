#!/usr/bin/env python3
# Agar.io prototype.

from trabant import *
from random import choice

def create_edible(pos, radius, col, mass):
	edible = create_sphere(pos=pos, radius=radius, col=col, mass=mass)
	edible.original_radius = edible.radius = radius
	return edible

def create_player(pos=vec3(), radius=1, col='#000'):
	player = create_edible(pos=pos, radius=radius, col=col, mass=radius)
	player.create_engine(push_abs_engine, strength=10, friction=1, max_velocity=5)
	return player

def kill(obj):
	global players, edibles, avatar
	obj.release()
	edibles.remove(obj)
	if obj in players:
		players.remove(obj)
		if avatar == obj:
			avatar = create_player(col='#0f0')
			players.add(avatar)
			edibles.add(avatar)
			cam(distance=35, target=avatar, smooth=0.9)

def grow(obj, eaten_radius):
	eaten_area = pi*eaten_radius**2
	new_radius = sqrt(obj.radius**2 + eaten_area/pi)
	obj.scale(new_radius / obj.original_radius)
	obj.radius = new_radius


bg('#fff')
create_box((0,0,+50+2), side=(100,4,4), static=True)
create_box((0,0,-50-2), side=(100,4,4), static=True)
create_box((+50+2,0,0), side=(4,4,108), static=True)
create_box((-50-2,0,0), side=(4,4,108), static=True)
gravity((0,0,0), bounce=0.04)
avatar = create_player(col='#0f0')
cam(distance=35, target=avatar, smooth=0.9)

players = [create_player(rndvec().with_y(0)*50, radius=1+random(), col='#f00') for _ in range(20)] + [avatar]
players = set(players)
edibles = set(players)

while loop():
	move = vec3(keydir().x, 0, keydir().y)
	avatar.engines[0].force(move)

	for obj1,obj2,_,_ in collisions():
		if obj1 in edibles and obj2 in edibles:
			d = (obj1.pos()-obj2.pos()).length()
			maxr = max(obj1.radius,obj2.radius)
			if d < maxr:
				if obj1.radius > obj2.radius:
					grow(obj1, obj2.radius)
					kill(obj2)
				else:
					grow(obj2, obj1.radius)
					kill(obj1)

	if timeout(0.1, timer='add_edible') and len(edibles) < 70:
		edibles.add(create_edible(rndvec().with_y(0)*50, radius=0.5, col='#00f', mass=100))

	if timeout(0.1, timer='movement'):
		opponent = choice(list(players))
		opponent.bounce_in_rect(vec3(-50,-0.1,-50), vec3(+50,+0.1,+50))
		if opponent != avatar:
			p = opponent.pos()
			if abs(p.x) > 40 or abs(p.z) > 40:
				opponent.engines[0].force((-p.x/abs(p.x),0,-p.z/abs(p.z)))
			else:
				opponent.engines[0].force(rndvec().with_y(0))
