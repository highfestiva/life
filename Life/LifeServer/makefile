# 'Twas generated from ../../Life/LifeServer/LifeServer.vcproj, 2009-12-26, type='bin'.
# Don't edit manually. See 'generate_makefile.py' for info.

CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS -I../../ThirdParty/stlport/stlport -I../../ThirdParty/utf8cpp -I../../ThirdParty/openal-soft-1.10.622/OpenAL32/Include -I../../ThirdParty/openal-soft-1.10.622/include -I../../ThirdParty/freealut-1.1.0/include -I../../ThirdParty/ode-0.11.1/include -I../../ThirdParty/ode-0.11.1/ode/src -I../../ThirdParty/ode-0.11.1/ode/src/joints -I../../ThirdParty/ode-0.11.1/OPCODE -I../../ThirdParty/ode-0.11.1/GIMPACT/include -I../../ThirdParty/ode-0.11.1/ou/include -DPOSIX -D_XOPEN_SOURCE=600 -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdSingle -DHAVE_CONFIG_H=1 -DLEPRA_WITHOUT_FMOD -Wall -Wno-unknown-pragmas

LIBS = -lLife -lCure -lTBC -lLepra -lThirdParty -lstlport -lpthread -ldl -lrt -L../../ThirdParty/stlport/build/lib/obj/gcc/so -L../../ThirdParty -L../../ThirdParty/openal-soft-1.10.622 -L../../ThirdParty/freealut-1.1.0/admin/VisualStudioDotNET/alut -L../../Lepra -L../../TBC -L../../Cure -L../../UiLepra -L../../UiTBC -L../../UiCure -L..

SRCS=	\
Client.cpp	\
GameServerManager.cpp	\
ServerApplication.cpp	\
ServerConsoleManager.cpp

OBJS=	\
Client.o	\
GameServerManager.o	\
ServerApplication.o	\
ServerConsoleManager.o

all:	LifeServer $(OBJS)
clean:
	@rm -f LifeServer $(OBJS)
LifeServer:	$(OBJS)
	g++ $(LIBS) -o $@ $(OBJS)

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)
	#@grep -Ev "\.o: .*([T]hirdParty|/[u]sr)/" makefile >.tmpmake
	#@mv .tmpmake makefile
.c.o:
	gcc $(CFLAGS) -o $@ -c $<
.cpp.o:
	g++ $(CFLAGS) -o $@ -c $<

