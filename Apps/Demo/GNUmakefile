
# Setup gti build environment.
ifndef GTISDKROOT
	include ../../Build/GTISDK.env
endif

ifeq ($(CROSS_COMPILE), 1)
$(info CROSS_COMPILE is $(CROSS_COMPILE))
FTDLIB = -l:libftd3xx.so -Wl,-rpath=$(LIBFTDI_PATH)
EXTRA_SYS_LIB =
else
CROSS_COMPILE=
# anticipate the library on an embedded platform is older
ifneq ($(CPU_ARCH),x86_64)
FTDLIB = -l:libftd3xx.so -Wl,-rpath=$(LIBFTDI_PATH)
EXTRA_SYS_LIB = -l:libstdc++.so.6
else
# config libopencv if on Ubuntu 16 or 18 (the path is Ubuntu specific)
# requires opencv 3.x
ifneq (,$(wildcard /usr/lib/x86_64-linux-gnu/libopencv_core.so.3*))
OPENCVLIB = `pkg-config opencv --cflags --libs`
else
OPENCVLIB = \
-I ../../Lib/$(OS_TYPE)/OpenCV/ \
-L ../../Lib/$(OS_TYPE)/OpenCV/$(CPU_ARCH)/ -l:libopencv_world.so.3.2 \
-Wl,-rpath=../../Lib/$(OS_TYPE)/OpenCV/$(CPU_ARCH)
endif
FTDLIB = -lftd3xx-static
EXTRA_SYS_LIB =
endif
endif

GTI_CC ?= g++

all: $(wildcard *.cpp) $(wildcard *.hpp)
	$(GTI_CC) -o demo -std=c++11 main.cpp -DLINUX -I ../../Include/ \
	$(OPENCVLIB) \
	-L ../../Lib/$(OS_TYPE)/$(CPU_ARCH)/ -lGTILibrary-static $(FTDLIB) -lgomp\
	$(EXTRA_SYS_LIB) -pthread -ldl

clean:
	rm demo

