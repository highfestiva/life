# 'Twas generated from ../../Life/LifeServer/LifeServer.vcproj, 2009-06-11, type='bin'.
# Don't edit manually. See 'generate_makefile.py' for info.

CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS -I../../ThirdParty/stlport/stlport -I../../ThirdParty/ode-060223/include -DPOSIX -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdCYLINDER_ENABLED -DCURE_TEST_WITHOUT_UI -Wall -Wno-unknown-pragmas

LIBS = -lLife -lCure -lTBC -lLepra -lThirdParty -lstlport -lpthread -ldl -lrt -L../../ThirdParty/stlport/build/lib/obj/gcc/so -L../../ThirdParty -L../../Lepra -L../../TBC -L../../Cure -L..

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

