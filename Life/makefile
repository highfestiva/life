# 'Twas generated from ../../Life/Life.vcproj, 2009-05-15, type='lib'.
# Don't edit manually. See 'generate_makefile.py' for info.

.SUFFIXES: .cpp
CFLAGS = -O0 -ggdb -c -D_POSIX_PTHREAD_SEMANTICS -IInclude -DPOSIX -D_DEBUG -D_CONSOLE -Wall

SRCS=	\
../../Life/ConsoleManager.cpp	\
../../Life/ContextResource.cpp	\
../../Life/LifeApplication.cpp

OBJS=	\
../../Life/ConsoleManager.o	\
../../Life/ContextResource.o	\
../../Life/LifeApplication.o

all:	$(OBJS)
clean:
	@rm -f $(OBJS)
depend:
	makedepend --$(CFLAGS) --$(SRCS)
.cpp.o:
	g++ $(CFLAGS) -c -o $@ $<
