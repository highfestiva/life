# 'Twas generated from ../../Life/Life.vcproj, 2009-05-25, type='lib'.
# Don't edit manually. See 'generate_makefile.py' for info.

CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS -I../ThirdParty/stlport/stlport -I../ThirdParty/ode-060223/include -DPOSIX -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdCYLINDER_ENABLED -Wall -Wno-unknown-pragmas

SRCS=	\
ConsoleManager.cpp	\
ContextResource.cpp	\
LifeApplication.cpp

OBJS=	\
ConsoleManager.o	\
ContextResource.o	\
LifeApplication.o

all:	libLife.so $(OBJS)
clean:
	@rm -f libLife.so $(OBJS)
libLife.so:	$(OBJS)
	g++ -shared -o $@ $(OBJS)

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)
.c.o:
	g++ $(CFLAGS) -o $@ -c $<
.cpp.o:
	g++ $(CFLAGS) -o $@ -c $<
