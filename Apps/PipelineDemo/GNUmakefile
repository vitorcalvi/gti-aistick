
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
FTDLIB = -lftd3xx-static
EXTRA_SYS_LIB =
endif
endif

GTI_CC ?= g++
TARGET = pipelineDemo

all: $(wildcard *.cpp) $(wildcard *.hpp)
	$(GTI_CC)  -o $(TARGET) -std=c++11 pipelineDemo.cpp -DLINUX -I ../../Include/ \
	-L ../../Lib/$(OS_TYPE)/$(CPU_ARCH)/ -lGTILibrary-static $(FTDLIB) -lgomp\
	$(EXTRA_SYS_LIB) -pthread -ldl 

clean:
	rm $(TARGET)

