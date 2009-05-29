# 'Twas generated from ../../TBC/TBC.vcproj, 2009-05-29, type='lib'.
# Don't edit manually. See 'generate_makefile.py' for info.

CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS -I../ThirdParty/stlport/stlport -I../ThirdParty/ode-060223/include -DPOSIX -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdCYLINDER_ENABLED -DCURE_TEST_WITHOUT_UI -Wall -Wno-unknown-pragmas

SRCS=	\
Source/Bones.cpp	\
Source/ChunkyBoneGeometry.cpp	\
Source/ChunkyLoader.cpp	\
Source/ChunkyStructure.cpp	\
Source/GeometryBase.cpp	\
Source/GeometryReference.cpp	\
Source/Model.cpp	\
Source/PhysicsEngineFactory.cpp	\
Source/PhysicsEngineODE.cpp	\
Source/PortalManager.cpp	\
Source/TBC.cpp	\
Source/TerrainFunction.cpp	\
Source/TerrainPatch.cpp

OBJS=	\
Source/Bones.o	\
Source/ChunkyBoneGeometry.o	\
Source/ChunkyLoader.o	\
Source/ChunkyStructure.o	\
Source/GeometryBase.o	\
Source/GeometryReference.o	\
Source/Model.o	\
Source/PhysicsEngineFactory.o	\
Source/PhysicsEngineODE.o	\
Source/PortalManager.o	\
Source/TBC.o	\
Source/TerrainFunction.o	\
Source/TerrainPatch.o

all:	libTBC.so $(OBJS)
clean:
	@rm -f libTBC.so $(OBJS)
libTBC.so:	$(OBJS)
	g++ -shared -o $@ $(OBJS)

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)
	#@grep -Ev "\.o: .*([T]hirdParty|/[u]sr)/" makefile >.tmpmake
	#@mv .tmpmake makefile
.c.o:
	gcc $(CFLAGS) -o $@ -c $<
.cpp.o:
	g++ $(CFLAGS) -o $@ -c $<

