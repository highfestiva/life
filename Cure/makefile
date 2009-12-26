# 'Twas generated from ../../Cure/Cure.vcproj, 2009-12-26, type='lib'.
# Don't edit manually. See 'generate_makefile.py' for info.

CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS -I../ThirdParty/stlport/stlport -I../ThirdParty/utf8cpp -I../ThirdParty/openal-soft-1.10.622/OpenAL32/Include -I../ThirdParty/openal-soft-1.10.622/include -I../ThirdParty/freealut-1.1.0/include -I../ThirdParty/ode-0.11.1/include -I../ThirdParty/ode-0.11.1/ode/src -I../ThirdParty/ode-0.11.1/ode/src/joints -I../ThirdParty/ode-0.11.1/OPCODE -I../ThirdParty/ode-0.11.1/GIMPACT/include -I../ThirdParty/ode-0.11.1/ou/include -DPOSIX -D_XOPEN_SOURCE=600 -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdSingle -DHAVE_CONFIG_H=1 -DLEPRA_WITHOUT_FMOD -Wall -Wno-unknown-pragmas

SRCS=	\
Source/ConsoleManager.cpp	\
Source/ContextManager.cpp	\
Source/ContextObject.cpp	\
Source/ContextObjectAttribute.cpp	\
Source/CppContextObject.cpp	\
Source/Cure.cpp	\
Source/GameManager.cpp	\
Source/NetworkAgent.cpp	\
Source/Packet.cpp	\
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
Source/CppContextObject.o	\
Source/Cure.o	\
Source/GameManager.o	\
Source/NetworkAgent.o	\
Source/Packet.o	\
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
	#@grep -Ev "\.o: .*([T]hirdParty|/[u]sr)/" makefile >.tmpmake
	#@mv .tmpmake makefile
.c.o:
	gcc $(CFLAGS) -o $@ -c $<
.cpp.o:
	g++ $(CFLAGS) -o $@ -c $<

