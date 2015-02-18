#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Binary API checking.

from trabant import *
from trabant.objects import orthoscale

trabant_init()


def mix_args(aa,bb):
	for a in aa:
		for b in bb:
			for i in range(len(a)):
				for j in range(i,len(a)+1):
					if i == j: yield True,a
					yield False,a[:i] + b[i:j] + a[j:]
	yield True,a

def protect(f, allow_fail, *args):
	try:
		return f(*args)
	except (ValueError,TypeError,AttributeError,AssertionError,IOError,TypeError) as e:
		if not allow_fail:
			print(f)
			print(*args)
			print('Should pass, but got:')
			raise e

def testrun(f,good,bad):
	print(f.__name__)
	return [protect(f, not ok, *args) for ok,args in mix_args(good,bad)][-1]


good = [(True,),(False,)]
bad = [('apa',),(6,)]
testrun(simdebug, good, bad)

good = [('hello',),('',)]
bad = [([],),(-8,)]
testrun(userinfo, good, bad)

good = [(0.1,)]
bad = [([],),(-1,),('1',)]
testrun(loop, good, bad)

good = [(0.1,)]
bad = [([],),(-1,)]
testrun(sleep, good, bad)

good = [(0.1,0,False),(0.01,1,True)]
bad = [('0','a','q'),(0,[],{})]
testrun(timeout, good, bad)

good = [('X',(1,2,3),(1,0,0,0),(5,4,3),(2,1,3),1,'#ff5','checker',False,False,None)]
bad = [('asbv',1,4,2,-1,'a',(3,),7,'a','b',''),('asbv',(1,),(4,5),(2,0,0,0),-1,'#aqQ','moz',protect,{},str)]
o = testrun(create_ascii_object, good, bad)

good = [((1,2,3),5,o,(1,2,3),45,True,(-3,-2,-1))]
bad = [((1,2,),{},-1,(2,3),-45,-1,(-3,-2,-1,5))]
testrun(cam, good, bad)
for _ in range(2):	# Reset cam twice for upcoming tests. Twice since it otherwise moves at the speed of light following the rouge object.
	cam(angle=(0,0,0),distance=20,target=0,pos=(0,0,0),fov=45,target_relative_angle=False,light_angle=(-1,0,-0.1))

good = [('#333',),('#FFAACC',),((0,1,0),)]
bad = [((1,2,3,),{},(-1,))]
testrun(bg, good, bad)

good = [((0,1,0),),('#333',True),('#FFAACC',False)]
bad = [((1,2,3,),{},(-1,))]
testrun(fg, good, bad)

good = [(0.1,10),(-10,1000)]
bad = [(-1,'100'),('a',{})]
testrun(fog, good, bad)

good = [((0,1,0),1,-1),((0,-1,100),None,0.3)]
bad = [((0,1,),'1',good),((0,1,2,3),[],{})]
testrun(gravity, good, bad)

good = [([(0,0,0),(1,0,0),(0,0,-1),(1,1,-1)], (0,1,2,2,1,3), (-1,-1,-1), (1,0,0,0), (-1,-1,-1), (1,1,1), 1, '#0ff', 'checker', False, None)]
bad = [(['a',(1,0,0),[]], (0,1,-2,'a'), [], (), {}, (0,), -1, 'fun', good, -3, '')]
testrun(create_mesh_object, good, bad)

good = [((1,0,0), (1,0,0,0), 2, (-1,-1,-1), (5,4,3), 1e10, 'smooth', '#f00', False, orthoscale((1,2,3)))]
bad = [('a', {}, (1,), [], (0,1,-2,'a'), [], {}, 0, -3, str)]
testrun(create_cube_object, good, bad)

good = [((0,-1,0), 2, (1,1,1), (-5,4,-3), 1e-10, '#369', 'flat', False, orthoscale((2,2,2)))]
bad = [('a', (1,), [], (0,1,-2,'a'), [], '_', {}, -3, list)]
testrun(create_sphere_object, good, bad)

good = [(o, [((1,1,1),(1,0,0,0))], 'flat', False)]
bad = [('a', (1,), [], '_')]
testrun(create_clones, good, bad)

good = [((0,0,0),(1,1,1), 0, 10)]
bad = [('a', (1,), [], '_')]
testrun(pick_objects, good, bad)

good = [[(0,0,0),(1,2,3),5],[(0,-1,0),(0,-2,0),0.1]]
bad = [[(0,0,),(-2,3),-5],['a',{},[]]]
testrun(explode, good, bad)

good = [[sound_clank,(0,0,0),(0,0,0),0.5], [sound_bang,(0,200,0),(0,30,0),100]]
bad = [['?',(0,0),(0,0),-0.5], ['apa.wav',[],{},'Q']]
testrun(sound, good, bad)

good = [[(10,20,-30)]]
bad = [[(0,0)], ['a']]
testrun(closest_click, good, bad)

good = [[(0.6,0.3),True]]
bad = [[(0.6,0.3,12),'?'], [(1,),{}]]
testrun(create_joystick, good, bad)

good = [[True],[False]]
bad = [['?'],[{}]]
testrun(accelerometer, good, bad)

# Create object
obj = create_ascii_object('XX')

# Test create some engine stuff.
good = [[push_abs_engine],[push_rel_engine],[push_turn_abs_engine],[push_turn_rel_engine]]
bad = [[roll_turn_engine],[roll_engine],[gyro_engine],[rotor_engine],[tilt_engine]]
engine = testrun(obj.create_engine, good, bad)

good = [[(1,1,1)],[-10]]
bad = [['a'],[(0,1,2,3)]]
testrun(engine.force, good, bad)

good = [[sound_clank,3,1],[sound_engine_rotor,0.5,10]]
bad = [['nosound',-1,-20],[[],'?',{}]]
testrun(engine.addsound, good, bad)

# Finally some raw object testing.
good = [[(0,0,0),(1,0,0,0)]]
bad = [[(0,0,0),(1,1,1,1)], [(0,0,0),(0,0,0)], [{},'apa']]
testrun(obj.pos, good, bad)

good = [[(1,0,0,0)]]
bad = [[(1,1,1,1)], [(0,0,0)], [{}]]
testrun(obj.orientation, good, bad)

good = [[(0,0,0),(1,0,0)]]
bad = [[(0,0,0),(1,1,1,1)], [('a',0,0),({},0,0)], [{},'knork']]
testrun(obj.vel, good, bad)

good = [[(1,2,3)]]
bad = [[(1,1,1,1)], ['qed'], [{}]]
testrun(obj.avel, good, bad)

good = [[1]]
bad = [[(1,2)], ['?'], [{}]]
testrun(obj.mass, good, bad)

good = [['#fff']]
bad = [[(1,2)], ['?'], [{}]]
testrun(obj.col, good, bad)

joint_obj = create_ascii_object('XX')
good = [[hinge_joint,joint_obj,(0,1,0),(-1,+1),(0.5,0.5)]]
bad = [['gack?',None,(0,0),(+1,-1,0),(-0.5,-0.5)], [{},{},{},'ab','cd']]
testrun(obj.joint, good, bad)

good = [[1]]
bad = [['a'], [{}]]
testrun(obj.add_stabilizer, good, bad)

obj.release()


######################################################################
# We're done. Try loading a game from scratch and see if it's working.
release_all_objects()
from trabant.gameapi import reset
reset()
import asteroids
