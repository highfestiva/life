# 'Twas generated 2009-05-23, type='base'.
# Don't edit manually. See 'generate_makefile.py' for info.

SRCS=	\
ThirdParty/libThirdParty.so	\
Lepra/libLepra.so	\
TBC/libTBC.so	\
Cure/libCure.so	\
Life/libLife.so

OBJS=	\
Life/LifeServer/LifeServer

all:	Life/LifeServer/LifeServer
Life/LifeServer/LifeServer:	$(SRCS)
	make --directory Life/LifeServer

ThirdParty/libThirdParty.so:
	make --directory ThirdParty
Lepra/libLepra.so:
	make --directory Lepra
TBC/libTBC.so:
	make --directory TBC
Cure/libCure.so:
	make --directory Cure
Life/libLife.so:
	make --directory Life
