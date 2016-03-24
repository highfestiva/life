from trabant import *

gravity((0,0,0))

def cap(pos,ang,r,l):
    o = quat().rotate_y((90-ang)*math.pi/180)
    return create_capsule(pos=pos,orientation=o,radius=r,length=l)

parts = [ cap((+0.0,+0.0,+0.0),  0,1.0,3.0),
          cap((+2.9,+0.0,+1.3), 45,0.4,0.7),
          cap((+3.7,+0.0,+2.5),  0,0.5,1.0),
          cap((+1.5,+0.5,-1.6), 90,0.2,0.8),
          cap((+1.5,-0.5,-1.6), 90,0.2,0.8),
          cap((+1.5,+0.5,-2.8), 90,0.2,0.6),
          cap((+1.5,-0.5,-2.8), 90,0.2,0.6),
          cap((-1.5,+0.5,-1.6), 90,0.2,0.8),
          cap((-1.5,-0.5,-1.6), 90,0.2,0.8),
          cap((-1.5,+0.5,-2.8), 90,0.2,0.6),
          cap((-1.5,-0.5,-2.8), 90,0.2,0.6),
          cap((-3.0,+0.0,+1.4),-45,0.2,1.4) ]

create_box((0,0,-24), side=40, static=True)

gravity((0,0,-10))

while loop():
    pass
