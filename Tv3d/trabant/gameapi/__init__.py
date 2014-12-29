#!/usr/bin/env python3


def open():

def reset():	# Kill all objects and joysticks. Set some default values.

def waitload(oid):

def isopen():



def cam(angle, distance, target, fov):

def fog(distance):

def gravity(g):

def explode(pos,vel):

def playsound(snd, pos, vel):



def pop_collisions():

def taps():

def accelerometer():

def joystick_data()::



def createobj(gfx,phys,static,pos):

def releaseobj(oid):

def create_engine(oid, engine_type, max_velocity, offset, sound)::

def create_joint(oid, joint_type, obj2, axis):



def setpos(oid, pos):

def getpos(oid):

def setorientation(oid, orientation):

def getorientation(oid):

def setvel(oid, vel):

def getvel(oid):

def setavel(oid, avel):

def getavel(oid):

def setweight(oid, w):

def getweight(oid):

def setcol(oid, col):

def getcol(oid):

def set_engine_force(oid, eid, xyz):
