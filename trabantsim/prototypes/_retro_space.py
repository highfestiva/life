#!/usr/bin/env python3
# Mah space game!

# Ideas:
#  + planets in the background,
#  + health,
#  + blink when immortal after death,
#  + tunnel,
#  + different & classical enemies,
#  + power-ups including weapons and shield,
#  + explosion effect when firing heavy weapons,
#  - bosses,
#  - scale objects - pulsating in size,
#  - slimy objects,
#  - 3D flying intermission mini-game.

import builtins
from trabant import *
from trabant.gameapi import setvar,waitload

gravity((0,0,0))
setvar('Phyics.FPS',60)

games = 'retro_gradius retro_planet_of_zoom'.split()
mods = [__import__(g) for g in games]

level,ship = 0,None
while True:
	userinfo('Level %i' % (level+1))
	timer_callback(0.5, userinfo)
	try:
		mod = mods[level%len(games)]
		ship = mod.play(level, ship)
		mod.transition(ship)
	except Exception as e:
		import traceback
		traceback.print_exc()
		#ship = []
		release_all_timers()
		release_objects()
		continue
	level += 1
