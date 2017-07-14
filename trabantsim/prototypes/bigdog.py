from trabant import *

floor = create_box(pos=(0,0,-15),side=27,static=True)

gravity((0,0,0))

def create_leg(body, p):
    upper = create_capsule(p+vec3(0,0,-0.2), radius=0.2, mass=3)
    lower = create_capsule(p+vec3(0,0,-0.8), radius=0.1, mass=1)
    body.joint(hinge_joint, upper, axis=(1,0,0))
    upper.joint(hinge_joint, lower, axis=(1,0,0))
    upper.servo = body.create_engine(roll_engine)
    lower.servo = upper.create_engine(roll_engine)
    return upper, lower

body = create_capsule(orientation=rotx(pi/2), radius=0.7, mass=5)
legs = []
for p in [(1,1,0),(1,-1,0),(-1,1,0),(-1,-1,0)]:
    legs += [create_leg(body, vec3(*p))]

gravity((0,0,-9.8))

while loop():
    for leg in legs:
        upper,lower = leg
        a = (lower.orientation() * vec3(0,0,1)).angle_x(vec3(0,0,1))
        lower.servo.force(sin(gametime())*0.3-a)
        a = (upper.orientation() * vec3(0,0,1)).angle_x(vec3(0,0,1))
        upper.servo.force(sin(gametime())*0.3-a)
