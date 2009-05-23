# 'Twas generated from ../../Life/LifeServer/LifeServer.vcproj, 2009-05-23, type='bin'.
# Don't edit manually. See 'generate_makefile.py' for info.

CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS -I../../ThirdParty/stlport/stlport -I../../ThirdParty/ode-060223/include -DPOSIX -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdCYLINDER_ENABLED -Wall -Wno-unknown-pragmas

LIBS = -lLife -lCure -lTBC -lLepra -lThirdParty -lm -lpthread -lnsl -lncurses -L../../ThirdParty -L../../Lepra -L../../TBC -L../../Cure -L..

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

all:	LifeServer
clean:
	@rm -f LifeServer $(OBJS)
LifeServer:	$(OBJS)
	g++ $(LIBS) -o $@ $(OBJS)

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)
.c.o:
	gcc $(CFLAGS) -o $@ -c $<
.cpp.o:
	g++ $(CFLAGS) -o $@ -c $<
