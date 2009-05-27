# 'Twas generated 2009-05-26, type='base'.
# Don't edit manually. See 'generate_makefile.py' for info.

SRCS=	\
ThirdParty	\
Lepra	\
TBC	\
Cure	\
Life

OBJS=	\
Life/LifeServer/LifeServer

all:	$(OBJS) $(SRCS)

clean:
	@rm bin/*
	$(MAKE) clean -C Life/LifeServer
	$(MAKE) clean -C ThirdParty
	$(MAKE) clean -C Lepra
	$(MAKE) clean -C TBC
	$(MAKE) clean -C Cure
	$(MAKE) clean -C Life

Life/LifeServer/LifeServer:	$(SRCS)
	$(MAKE) -C Life/LifeServer
	@cp ThirdParty/stlport/build/lib/obj/gcc/so/libstlport.so.5.2 bin/
	@cp $@ bin/


.PHONY:	$(OBJS) $(SRCS)

$(SRCS):
	$(MAKE) -C $@
	@rm -f $(OBJS)
	@cp $@/*.so bin/
