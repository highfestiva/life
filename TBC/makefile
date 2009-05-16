# 'Twas generated from ../../TBC/TBC.vcproj, 2009-05-15, type='lib'.
# Don't edit manually. See 'generate_makefile.py' for info.

.SUFFIXES: .cpp
CFLAGS = -O0 -ggdb -c -D_POSIX_PTHREAD_SEMANTICS -IInclude -DPOSIX -D_DEBUG -D_CONSOLE -Wall

SRCS=	\
../../TBC/Source/Bones.cpp	\
../../TBC/Source/ChunkyBoneGeometry.cpp	\
../../TBC/Source/ChunkyLoader.cpp	\
../../TBC/Source/ChunkyStructure.cpp	\
../../TBC/Source/GeometryBase.cpp	\
../../TBC/Source/GeometryReference.cpp	\
../../TBC/Source/Model.cpp	\
../../TBC/Source/PhysicsEngineFactory.cpp	\
../../TBC/Source/PhysicsEngineODE.cpp	\
../../TBC/Source/PortalManager.cpp	\
../../TBC/Source/TBC.cpp	\
../../TBC/Source/TerrainFunction.cpp	\
../../TBC/Source/TerrainPatch.cpp

OBJS=	\
../../TBC/Source/Bones.o	\
../../TBC/Source/ChunkyBoneGeometry.o	\
../../TBC/Source/ChunkyLoader.o	\
../../TBC/Source/ChunkyStructure.o	\
../../TBC/Source/GeometryBase.o	\
../../TBC/Source/GeometryReference.o	\
../../TBC/Source/Model.o	\
../../TBC/Source/PhysicsEngineFactory.o	\
../../TBC/Source/PhysicsEngineODE.o	\
../../TBC/Source/PortalManager.o	\
../../TBC/Source/TBC.o	\
../../TBC/Source/TerrainFunction.o	\
../../TBC/Source/TerrainPatch.o

all:	$(OBJS)
clean:
	@rm -f $(OBJS)
depend:
	makedepend --$(CFLAGS) --$(SRCS)
.cpp.o:
	g++ $(CFLAGS) -c -o $@ $<
