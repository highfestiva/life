# 'Twas generated from ../../Cure/Cure.vcproj, 2009-05-25, type='lib'.
# Don't edit manually. See 'generate_makefile.py' for info.

CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS -I../ThirdParty/stlport/stlport -I../ThirdParty/ode-060223/include -DPOSIX -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdCYLINDER_ENABLED -Wall -Wno-unknown-pragmas

SRCS=	\
Source/ConsoleManager.cpp	\
Source/ContextManager.cpp	\
Source/ContextObject.cpp	\
Source/ContextObjectAttribute.cpp	\
Source/ContextObjectEngine.cpp	\
Source/CppContextObject.cpp	\
Source/Cure.cpp	\
Source/GameManager.cpp	\
Source/NetworkAgent.cpp	\
Source/Packer.cpp	\
Source/Packet.cpp	\
Source/PhysicsNode.cpp	\
Source/PositionalData.cpp	\
Source/ResourceManager.cpp	\
Source/RuntimeVariable.cpp	\
Source/TerrainFunctionManager.cpp	\
Source/TerrainManager.cpp	\
Source/TerrainPatchLoader.cpp	\
Source/TerrainPatchManager.cpp	\
Source/TimeManager.cpp	\
Source/NetworkClient.cpp	\
Source/NetworkServer.cpp	\
Source/UserAccount.cpp	\
Source/UserAccountManager.cpp	\
Source/UserConnection.cpp

OBJS=	\
Source/ConsoleManager.o	\
Source/ContextManager.o	\
Source/ContextObject.o	\
Source/ContextObjectAttribute.o	\
Source/ContextObjectEngine.o	\
Source/CppContextObject.o	\
Source/Cure.o	\
Source/GameManager.o	\
Source/NetworkAgent.o	\
Source/Packer.o	\
Source/Packet.o	\
Source/PhysicsNode.o	\
Source/PositionalData.o	\
Source/ResourceManager.o	\
Source/RuntimeVariable.o	\
Source/TerrainFunctionManager.o	\
Source/TerrainManager.o	\
Source/TerrainPatchLoader.o	\
Source/TerrainPatchManager.o	\
Source/TimeManager.o	\
Source/NetworkClient.o	\
Source/NetworkServer.o	\
Source/UserAccount.o	\
Source/UserAccountManager.o	\
Source/UserConnection.o

all:	libCure.so $(OBJS)
clean:
	@rm -f libCure.so $(OBJS)
libCure.so:	$(OBJS)
	g++ -shared -o $@ $(OBJS)

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)
.c.o:
	g++ $(CFLAGS) -o $@ -c $<
.cpp.o:
	g++ $(CFLAGS) -o $@ -c $<
