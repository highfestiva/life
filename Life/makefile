# 'Twas generated from ../../Life/Life.vcproj, 2009-12-26, type='lib'.
# Don't edit manually. See 'generate_makefile.py' for info.

CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS -I../ThirdParty/stlport/stlport -I../ThirdParty/utf8cpp -I../ThirdParty/openal-soft-1.10.622/OpenAL32/Include -I../ThirdParty/openal-soft-1.10.622/include -I../ThirdParty/freealut-1.1.0/include -I../ThirdParty/ode-0.11.1/include -I../ThirdParty/ode-0.11.1/ode/src -I../ThirdParty/ode-0.11.1/ode/src/joints -I../ThirdParty/ode-0.11.1/OPCODE -I../ThirdParty/ode-0.11.1/GIMPACT/include -I../ThirdParty/ode-0.11.1/ou/include -DPOSIX -D_XOPEN_SOURCE=600 -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdSingle -DdTLS_ENABLED=1 -DHAVE_CONFIG_H=1 -DLEPRA_WITHOUT_FMOD -Wall -Wno-unknown-pragmas

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

