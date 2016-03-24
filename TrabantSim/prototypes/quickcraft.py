# A minimal 22 LoC Minecraft imitation for the Trabant API (pixeldoctrine.com/trabant.html).
# Note that this is made for computers and no touch controls are included.

from trabant import *

# draw all shapes filled
fg(outline=False)

# floor
for y in range(5):
    for x in range(5):
        create_box((x-2,y-2,0), static=True, col=rndvec().abs(), mat='noise')

# avatar
me = create_sphere((0,0,2), radius=0.5, col='#0000')
cam(target=me, distance=0)
yaw,pitch = 0,0

while loop():   # main loop
    # mlook
    yaw,pitch = yaw-mousemove().x*0.1,pitch-mousemove().y*0.1
    cam(angle=(pitch,0,yaw))

    # movement
    xyrot = quat().rotate_z(yaw)
    me.vel(xyrot*keydir())

    # place and remove blocks
    if click() and timeout(0.2):
        pos,direction = me.pos(),xyrot.rotate_x(pitch)*vec3(0,1,0)
        clicked_objects = [(o,p) for o,p in pick_objects(pos,direction,0,6) if o!=me]
        if clicked_objects:
            box,hitpos = clicked_objects[0]    # nearest box = clicked box
            if click(left=True):    # left lick = remove box
                box.release()
            else:   # right click = add box
                create_box(box.pos() + toaxis(hitpos-box.pos()), static=True, col=rndvec().abs(), mat='noise')
