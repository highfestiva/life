# 'Twas generated from ../../TBC/TBC.vcproj, 2009-12-22, type='lib'.
# Don't edit manually. See 'generate_makefile.py' for info.

CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS -I../ThirdParty/stlport/stlport -I../ThirdParty/utf8cpp -I../ThirdParty/ode-0.11.1/include -I../ThirdParty/ode-0.11.1/ode/src -I../ThirdParty/ode-0.11.1/ode/src/joints -I../ThirdParty/ode-0.11.1/OPCODE -I../ThirdParty/ode-0.11.1/GIMPACT/include -I../ThirdParty/ode-0.11.1/ou/include -DPOSIX -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdCYLINDER_ENABLED -DCURE_TEST_WITHOUT_UI -Wall -Wno-unknown-pragmas

SRCS=	\
Source/Bones.cpp	\
Source/ChunkyBoneGeometry.cpp	\
Source/ChunkyClass.cpp	\
Source/ChunkyLoader.cpp	\
Source/ChunkyPhysics.cpp	\
Source/GeometryBase.cpp	\
Source/GeometryReference.cpp	\
Source/Model.cpp	\
Source/PhysicsEngine.cpp	\
Source/PhysicsManagerFactory.cpp	\
Source/PhysicsManagerODE.cpp	\
Source/PortalManager.cpp	\
Source/TBC.cpp	\
Source/TerrainFunction.cpp	\
Source/TerrainPatch.cpp

OBJS=	\
Source/Bones.o	\
Source/ChunkyBoneGeometry.o	\
Source/ChunkyClass.o	\
Source/ChunkyLoader.o	\
Source/ChunkyPhysics.o	\
Source/GeometryBase.o	\
Source/GeometryReference.o	\
Source/Model.o	\
Source/PhysicsEngine.o	\
Source/PhysicsManagerFactory.o	\
Source/PhysicsManagerODE.o	\
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

