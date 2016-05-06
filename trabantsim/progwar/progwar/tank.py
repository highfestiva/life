from trabant import *
from trabant.gameapi import waitload
from trabant.mesh import mesh_union
from trabant.objects import *
from trabant.objgen import createcapsule
from types import MethodType


body = r'''
    /XX\
<XXXXXXXXX\
 ´XXXXXXXXX>
'''
template_tank = None


def _add_canon(orientation,gfx,phys):
    canon = createcapsule(0.4, 6, 2, 4)[0]
    gfx.vertices,gfx.indices = mesh_union(gfx.vertices, gfx.indices, canon.vertices, canon.indices, quat().rotate_y(pi/2), vec3(-4,-2,0))
    return orientation,gfx,phys

def init_tank_obj():
    global template_tank
    preprocess_tank = process_chain(orthoscale((1,1.6,6)), process_rot((0,0,-pi/2)), _add_canon)
    template_tank = create_ascii_object(body, pos=vec3(1000,1000,1000), static=True, process=preprocess_tank)

def update_tank(self, vel):
    vel = vel.with_z(0)
    if vel.length() > 0.5:
        direction = vel.normalize()
        yaw = -atan2(direction.x, direction.y)
        self.orientation(quat().rotate_z(yaw))
        self.avel(vec3())

def create_tank_obj(pos, col, vel=None):
    obj = create_clones(template_tank, [(pos,quat())])[0]
    gameapi.waitload(obj.id)
    obj.vel(vel)
    obj.col(col)
    obj.update_tank = MethodType(update_tank, obj)
    return obj

def process_tank_floor(orientation, gfx, phys):
    top = max(gfx.vertices, key=lambda v:v.z).z
    bottom = top-30
    for v in gfx.vertices:
        v.z = max(v.z,bottom)
    return orientation,gfx,phys
