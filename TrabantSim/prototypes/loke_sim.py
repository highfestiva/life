from trabant import *

fg(outline=False)
 
#bus with wheels
bus = create_box(side=(6,1.5,2))
rear_left   = create_sphere(pos=(-3,+1,-1), radius=0.25)
rear_right  = create_sphere(pos=(-3,-1,-1), radius=0.25)
front_left  = create_sphere(pos=(+3,-1,-1), radius=0.25)
front_right = create_sphere(pos=(+3,+1,-1), radius=0.25)
bus.joint(suspend_hinge_joint, rear_left, (0,+1,0))
bus.joint(suspend_hinge_joint, rear_right, (0,+1,0))
bus.joint(suspend_hinge_joint, front_left, (0,+1,0))
bus.joint(suspend_hinge_joint, front_right, (0,+1,0))
bus.vel((5,0,0))

#floor
create_box(pos=(8,4,-23), side=40, static=True, col='#0f0', mat='flat')

for z in range(0,7):
    create_box(pos=(5,4,z))

create_sphere(pos=(5,0,3), vel=(0,99,2))

while loop():
    pass
