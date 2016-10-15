#!/usr/bin/env python3
# GTA-like car chase.

from trabant import *
from trabant.objects import process_chain,gfx_ortho_pinch,gfxrot

# ASCII geometry for the pickup. An X is one unit wide, high and deep.
chassis = r'''
    XX\
XXXXXXXXX
XXXXXXXXX
'''
chassis = '\n---\n'.join([chassis]*3)    # Make the car a bit wider.

bg(col='#5af')
# Create objects floating in mid-air, or joints will be off when things start falling down before they're bolted in.
gravity((0,0,0), friction=1)
collisions(False)

def create_brake_lights(car, pos):
    car._brake_lights = [create_box(vec3(*pos)+vec3(*v), side=0.5, col='#300', mat='flat') for v in [(-4.9,+1.2,-0.3),(-4.9,-1.2,-0.3)]]
    car.joint(fixed_joint, car._brake_lights[0])
    car.joint(fixed_joint, car._brake_lights[1])
    car.brake_lights = lambda c: [bl.col(c) for bl in car._brake_lights]

def create_wheels(body, pos):
    # The graphics are processed while generating: since the sphere vertices are generated symmetrically
    # around the Z-axis, and the relative vertex density is higher near the north- and south poles, we
    # first rotate the sphere 90 degrees along the X-axis. Then we "pinch" the sphere using a sinus
    # function to generate something resembling a car wheel.
    wheelshape = gfx_ortho_pinch(0,1,0, func=lambda a:abs(sin(a)**3)*0.5-0.7)
    process_gfx_spheres_to_wheels = process_chain(gfxrot(pi/2,0,0), wheelshape)
    wheel = lambda x,y,z: create_sphere(pos=vec3(*pos)+vec3(x,y,z), col='#111', process=process_gfx_spheres_to_wheels)
    rr,rl,fr,fl = wheel(-3.5,-1.8,-1.5), wheel(-3.5,+1.8,-1.5), wheel(2.6,-1.6,-1.5), wheel(2.6,+1.6,-1.5)
    suspension_spring,suspension_damping = 0.1,2
    body.joint(turn_hinge_joint, fl, axis=(0,-1,0), spring=(suspension_spring,suspension_damping))
    body.joint(turn_hinge_joint, fr, axis=(0,-1,0), spring=(suspension_spring,suspension_damping))
    body.joint(suspend_hinge_joint, rl, axis=(0,-1,0), spring=(suspension_spring,suspension_damping))
    body.joint(suspend_hinge_joint, rr, axis=(0,-1,0), spring=(suspension_spring,suspension_damping))
    return rr,rl,fr,fl

def create_car(pos, col, name):
    # A body, four wheels and three actuators: acceleration, turn and braking.
    body = create_ascii_object(chassis, pos=vec3(*pos), mass=200, col=col)
    body.name = name
    create_brake_lights(body, pos)
    rr,rl,fr,fl = create_wheels(body, pos)
    body.turn = body.create_engine(roll_turn_engine, targets=[(fl,1),(fr,1)])
    body.roll = body.create_engine(roll_engine, targets=[(rl,1),(rr,1)], strength=0.4, sound=sound_engine_combustion)
    body.brake = body.create_engine(roll_brake_engine, targets=[(rl,-1),(rr,-1),(fl,1),(fr,1)], strength=0.2, sound=sound_engine_combustion)
    body.create_engine(upright_stabilize)
    return body

def create_ground_patch(x, y):
    ground = create_box((x+35,y+35,-50-2.3), side=100, static=True)
    house = create_box((x+50,y+50,20-2.3), side=(70,70,40), col=rndvec().abs(), mat='flat', static=True)
    return [ground,house]

def update_terrain(pos):
    cx,cy = int((pos.x-50)/100), int((pos.y-50)/100)
    remove_patches = set(ground_patches.keys())
    # Terrain is updated from center outwards.
    for yo in [0,1,-1,2,-2,3,-3]:
        for xo in [0,1,-1,2,-2,3,-3]:
            if abs(yo)>=2 and abs(xo)>=2:
                continue # Just do two corridors: north-south and east-west.
            x = cx + xo
            y = cy + yo
            patch_id = y*757+x
            if patch_id not in ground_patches:
                ground_patches[patch_id] = create_ground_patch(x*100,y*100)
                return
            else:
                remove_patches.remove(patch_id)
    for patch_id in remove_patches:
        [o.release() for o in ground_patches[patch_id]]
        del ground_patches[patch_id]

me = create_car((30,0,0), '#cc5', 'myself')
ai_cars = create_car((100,0,0), '#000', 'black'), create_car((-40,0,0), '#11b', 'blue')
ground_patches = {}
[update_terrain(vec3(30,0,0)) for _ in range(4)]

# Setup ground, gravity, camera angle and controls.
collisions(True)
gravity((0,0,-9))    # Allow objects to fall down now that they're attached by joints.
camang = vec3(-0.4,0,+0.2-pi/2)
cam(angle=camang, target=me, pos=(0,3,0), target_relative_angle=True, smooth=0.8, light_angle=(-1.3,0,0.1))
left_stick,right_stick = create_joystick((0,0)),create_joystick((1,0))

def ai_reverse_due_to_obstruction(car):
    # Check if obstructed, and if so we start the reverse timer. We're obstructed when we've had a low
    # velocity for some time. The reverse timer will keep us backing up for some time; it takes a little
    # bit of time to reverse a few meters.
    is_obstructed = car.vel().length2() < 1 and timeout(1, 'obstruction'+str(car))
    if is_obstructed:
        timein_restart('reverse'+str(car)) # Start backing up.
    # Back up for some time if it's decided we've been obstructed.
    if timein(1, 'reverse'+str(car), auto_start=False):
        timeout_restart('obstruction'+str(car)) # We're not is_obstructed while we reverse.
        return True
    return False

def ai_drive(car):
    v = (me.pos()+me.vel() - car.pos()).normalize()
    forward = car.orientation() * vec3(1,0,0)
    should_go_fwd = (v * forward > 0)
    # Inverse direction if we're obstructed.
    should_go_fwd ^= ai_reverse_due_to_obstruction(car)
    car.roll.force(1 if should_go_fwd else -1)
    # Brake control.
    is_moving_forward = (car.vel() * (car.orientation()*vec3(1,0,0)) > -0.1)
    if not should_go_fwd and is_moving_forward:
        car.brake.force(control_fwd)
        car.brake_lights('#f22f8')
    else:
        car.brake.force(0)
        car.brake_lights('#300')
    # Control steering wheel.
    angle = -forward.angle_z(v)
    car.turn.force(angle if should_go_fwd else -angle/abs(angle))
    # If we fall too far behind, the ground patches beneath us will be removed.
    got_lost = car.pos().z < -1  # Fell down when ground was taken away underneath us.
    if got_lost and timeout(1):
        userinfo('Got away from %s!' % car.name)
        # When the player gets away from one of the two gangsters, we teleport it up ahead.
        ai_teleport_car(car)

def ai_teleport_car(car):
    car.vel((0,0,0))
    # Find the closest street 100 units ahead of the player.
    ahead_of_me = me.pos() + me.vel().with_z(0).normalize(100)
    closest_street = vec3(*[round(v/100)*100 for v in ahead_of_me])
    if abs(closest_street.x-ahead_of_me.x) < abs(closest_street.y-ahead_of_me.y):
        ahead_of_me.x = closest_street.x
    else:
        ahead_of_me.y = closest_street.y
    car.pos(ahead_of_me)

while loop():
    # Player acceleration and turning.
    control_fwd = left_stick.y + keydir().y
    me.roll.force(control_fwd)
    me.turn.force(right_stick.x + keydir().x)
    # Player brake control.
    is_moving_forward = (me.vel() * (me.orientation()*vec3(1,0,0)) > -0.1)
    if is_moving_forward and control_fwd < 0:
        me.brake.force(control_fwd)
        me.brake_lights('#f22f8')
    else:
        me.brake.force(0)
        me.brake_lights('#300')
    # Camera control. Look backwards when player is backing up.
    look_forward = is_moving_forward ^ ('Space' in keys())
    cam(angle=camang if look_forward else camang+vec3(0,0,pi*0.9))

    update_terrain(me.pos())

    for car in ai_cars:
        ai_drive(car)
