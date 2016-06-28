#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Planet+moons simulation.

from trabant import *

fg(outline=False)
cam(distance=65, angle=(-0.2,0,0), light_angle=(-0.05,0,0.3))
gravity((0,0,0))

# Create a small gravity simulation of mars.
mars = create_sphere(radius=2.5, col='#c83', avel=(0.01,0,0.3), mass=1e6)
mars.create_engine(push_abs_engine)
phobos = create_sphere(pos=(-1,18,-1), radius=0.3, col='#789', avel=(0.01,0.01,1), mass=1e3)
phobos.create_engine(push_abs_engine)
deimos = create_sphere(pos=(1,-25,0), radius=0.3, col='#897', avel=(0.01,0.01,1), mass=1e3)
deimos.create_engine(push_abs_engine)

phobos.vel((15,0,0))
deimos.vel((-12,0,0))
while loop():
    dir1 = phobos.pos() - mars.pos()
    dir2 = deimos.pos() - mars.pos()
    dir3 = phobos.pos() - deimos.pos()
    # These constants are just plucked out of thin air.
    f1 = dir1.normalize() * 4000/(dir1.length()**2)
    f2 = dir2.normalize() * 4000/(dir2.length()**2)
    f3 = dir3.normalize() * 4/(dir3.length()**2)
    # All forces applied to engines are adjusted to mass, so we have to scale down accordingly.
    mars.engines[0].force((f1+f2)*1e-3)
    phobos.engines[0].force(-f1-f3)
    deimos.engines[0].force(-f2+f3)
