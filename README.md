Life - a basic 3D physics game engine
-----------------------------------------------------

I've spent many years writing this engine, but if you want advanced rendering, character animation and skinning, you should go with UE4. If you're more interested in vehicles, robots and simulations in 3D, this engine might prove helpful.

Life is portable and supports Windows, X11, MacOS and iOS. I've released [five](http://pixeldoctrine.com/) (unsuccessful) games with it over the years.

I have no tutorials, but there are a couple of finished games in the repo. As of late I've been building with [CodeLite](http://www.codelite.org/) and llvm on Ubuntu. Just open [life/life.workspace](life/life.workspace) and hit compile.

Build the assets and start a game like so:
`$ tools/build/rgo.py set_target <game> "Game Name"`
`$ tools/build/rgo.py builddata`
`$ cd <game>`
`$ Debug/<game>`

If you want to ship closed-source you must build the [thirdparty/](thirdparty/) code as a separate .dll/.so as some parts are LGPL.
