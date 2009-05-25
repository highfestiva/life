# 'Twas generated 2009-05-25, type='base'.
# Don't edit manually. See 'generate_makefile.py' for info.

SRCS=	\
ThirdParty/libThirdParty.so	\
Lepra/libLepra.so	\
TBC/libTBC.so	\
Cure/libCure.so	\
Life/libLife.so

OBJS=	\
Life/LifeServer/LifeServer

all:	$(OBJS) $(SRCS)

clean:
	@rm bin/*
	make clean --directory Life/LifeServer
	make clean --directory ThirdParty
	make clean --directory Lepra
	make clean --directory TBC
	make clean --directory Cure
	make clean --directory Life

Life/LifeServer/LifeServer:	$(SRCS)
	make --directory Life/LifeServer
	@cp ThirdParty/stlport/build/lib/obj/gcc/so/libstlport.so.5.2 bin/
	@cp Life/LifeServer/LifeServer bin/

ThirdParty/libThirdParty.so:
	make --directory ThirdParty
	@cp ThirdParty/libThirdParty.so bin/
Lepra/libLepra.so:
	make --directory Lepra
	@cp Lepra/libLepra.so bin/
TBC/libTBC.so:
	make --directory TBC
	@cp TBC/libTBC.so bin/
Cure/libCure.so:
	make --directory Cure
	@cp Cure/libCure.so bin/
Life/libLife.so:
	make --directory Life
	@cp Life/libLife.so bin/
