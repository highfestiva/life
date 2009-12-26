# 'Twas generated 2009-12-26, type='base'.
# Don't edit manually. See 'generate_makefile.py' for info.

SRCS=	\
ThirdParty	\
ThirdParty/openal-soft-1.10.622	\
ThirdParty/freealut-1.1.0/admin/VisualStudioDotNET/alut	\
Lepra	\
TBC	\
Cure	\
UiLepra	\
UiTBC	\
UiCure	\
Life

OBJS=	\
Life/LifeServer	\
Life/LifeClient	\
UiCure/CureTestApp

BINS=	\
Life/LifeServer/LifeServer	\
Life/LifeClient/LifeClient	\
UiCure/CureTestApp/CureTestApp


.PHONY:	$(BINS) $(OBJS) $(SRCS) all clean depend

all:	$(OBJS) $(SRCS) $(BINS)

clean:
	@rm -f bin/*
	@for SUBDIR in $(SRCS); do \
		$(MAKE) -C $$SUBDIR clean; \
	done
	@for SUBDIR in $(OBJS); do \
		$(MAKE) -C $$SUBDIR clean; \
	done

depend:
	@for SUBDIR in $(SRCS); do \
		$(MAKE) -C $$SUBDIR depend; \
	done
	@for SUBDIR in $(OBJS); do \
		$(MAKE) -C $$SUBDIR depend; \
	done

$(BINS):	$(OBJS)
	@cp ThirdParty/stlport/build/lib/obj/gcc/so/libstlport.so.5.2 bin/
	@cp $@ bin/

$(OBJS):	$(SRCS)
	$(MAKE) -C $@

$(SRCS):
	$(MAKE) -C $@
	@rm -f $(BINS)
	@cp $@/*.so bin/
