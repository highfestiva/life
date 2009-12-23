# 'Twas generated from ../../Life/Life.vcproj, 2009-12-22, type='lib'.
# Don't edit manually. See 'generate_makefile.py' for info.

CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS -I../ThirdParty/stlport/stlport -I../ThirdParty/utf8cpp -I../ThirdParty/ode-0.11.1/include -I../ThirdParty/ode-0.11.1/ode/src -I../ThirdParty/ode-0.11.1/ode/src/joints -I../ThirdParty/ode-0.11.1/OPCODE -I../ThirdParty/ode-0.11.1/GIMPACT/include -I../ThirdParty/ode-0.11.1/ou/include -DPOSIX -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdCYLINDER_ENABLED -DCURE_TEST_WITHOUT_UI -Wall -Wno-unknown-pragmas

SRCS=	\
ConsoleManager.cpp	\
LifeApplication.cpp

OBJS=	\
ConsoleManager.o	\
LifeApplication.o

all:	libLife.so $(OBJS)
clean:
	@rm -f libLife.so $(OBJS)
libLife.so:	$(OBJS)
	g++ -shared -o $@ $(OBJS)

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)
	#@grep -Ev "\.o: .*([T]hirdParty|/[u]sr)/" makefile >.tmpmake
	#@mv .tmpmake makefile
.c.o:
	gcc $(CFLAGS) -o $@ -c $<
.cpp.o:
	g++ $(CFLAGS) -o $@ -c $<

