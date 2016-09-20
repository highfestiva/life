#!/usr/bin/env python3
# Candy Crush 9x9 prototype.

from trabant import *
from itertools import groupby
from random import randint

grid = [[None]*9 for _ in range(9)]

bg('#379')
cam(distance=13)

def create_candy(pos, kind):
	if kind == 1: obj = create_capsule(pos+vec3(-4,0,-4), orientation=roty(-pi/4), radius=1/5, length=1/2, col='#812', static=True)
	if kind == 2: obj = create_sphere(pos+vec3(-4,0,-4), radius=0.4, col='#28c', static=True)
	if kind == 3: obj = create_sphere(pos+vec3(-4,0,-4), radius=0.3, col='#ff0', static=True)
	if kind == 4: obj = create_capsule(pos+vec3(-4,0,-4), radius=0.3, length=0.3, col='#c72', static=True)
	if kind == 5: obj = create_sphere(pos+vec3(-4,0,-4), radius=0.4, col='#f4f', static=True)
	if kind == 6: obj = create_box(pos+vec3(-4,0,-4), side=0.8, col='#5e4', mat='flat', static=True)
	obj.kind = kind
	return obj

def eat():
	'''Remove all pieces which are three or more of the same kind in a row.'''
	eaten = set()
	for rowcol in (grid, zip(*grid)): # Walk both rows and columns.
		for row in rowcol:
			# Find all groups in a row with >= 3 in a row.
			for kind,group in groupby(row, lambda o: o.kind if o else -1):
				if kind <= 0:
					continue
				group = list(group)
				if len(group) >= 3:
					eaten = eaten.union(group)
	# Remove all objects that appeared as 3-in-a-row.
	for obj in eaten:
		p = intvec3(obj.pos()+vec3(4,0,4))
		grid[p.z][p.x] = None
		obj.release()
	return eaten

def fall_and_refill():
	# Fall down row by row.
	for rowi in range(8):
		for coli,o in enumerate(grid[rowi]):
			if not o:
				#print(coli, rowi+1, rowi)
				grid[rowi][coli],grid[rowi+1][coli] = grid[rowi+1][coli],None
				updatepiece(vec3(coli,0,rowi))
	# Refill new pieces at top.
	toprow = [(o if o else create_candy(vec3(x,0,8), randint(1,6))) for x,o in enumerate(grid[-1])]
	grid[-1] = toprow

def updatepiece(gridpos, off=vec3()):
	'''Updates the position of the piece at the given grid coordinate.'''
	if grid[gridpos.z][gridpos.x]:
		grid[gridpos.z][gridpos.x].pos(gridpos+vec3(-4,0,-4)+off)

def tap2griddrag(tap):
	'''Returns the start and end (+-1 horizontal or vertical) position of the drag in grid-coords.'''
	def pos2grid(pos):
		return intvec3(vec3(clamp(round(pos.x),-4,4), 0, clamp(round(pos.z),-4,4)) + vec3(4,0,4))
	start,end = pos2grid(tap.startpos3d()),pos2grid(tap.pos3d())
	direction = end - start
	if direction.length() < 0.2:
		return start,start
	direction = vec3(direction.x, 0, 0) if abs(direction.x) > abs(direction.z) else vec3(0, 0, direction.z)
	return start,start+intvec3(direction.normalize())

while loop(delay=0.1): # Slightly longer delays to compensate for lack of animations.
	fall_and_refill()
	eat()

	# Drag and drop.
	for tap in taps():
		frompos,topos = tap2griddrag(tap)
		if tap.ispress:
			# Drag towards next square.
			updatepiece(frompos, (topos-frompos)*0.3)
		else:
			# Switch two pieces if possible.
			grid[frompos.z][frompos.x],grid[topos.z][topos.x] = grid[topos.z][topos.x],grid[frompos.z][frompos.x]
			updatepiece(frompos)
			updatepiece(topos)
			if not eat():
				# Not three in a row, so switch both treats back.
				grid[frompos.z][frompos.x],grid[topos.z][topos.x] = grid[topos.z][topos.x],grid[frompos.z][frompos.x]
				updatepiece(frompos)
				updatepiece(topos)
