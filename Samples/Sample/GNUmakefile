# ****************************************************************
# *                    Copyright (c) 2018                        *
# *         Gyrfalcon technology Inc. All Rights Reserved        *
# ****************************************************************
#
#
# Setup gti build environment.
ifndef GTISDKROOT
	include ../../Build/GTISDK.env
endif

#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

# define the C compiler to use
GTI_CC ?= g++

# define the lib.a creater to use
GTI_LIBAR ?= ar

# define the lib.a creater flags
AFLAGS = rcs

# define any compile-time flags
#CFLAGS = -Wall -g -std=gnu++11
CFLAGS = -Wall -O3 -fPIC -std=gnu++11 -DLINUX #-DFACE_RECOGNITION 
#CFLAGS += -DUSE_FTDI
CFLAGS += -DUSE_EUSB
#CFLAGS += -DUSE_PCIE

#CFLAGS += -DGTI2803


# define any directories containing header files other than /usr/include
#
INCLUDES = -I$(INCOPENCV_PATH) -I$(GTISDKROOT)/Include

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS += -L$(GTILIB_PATH)
LFLAGS += -L$(LIBFTDI_PATH)
LFLAGS += -L$(LIBOPENCV_PATH)
LFLAGS += -lgomp

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
#LIBS = -lmylib -lm
#LIBS = -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_videoio -lGTILibrary-static -lftd3xx -lpthread -lX11
#add static link for cross compile
ifeq ($(CROSS_COMPILE), 1)
	LIBS = -ldl -lpthread
	LIBS += -lGTILibrary-static -Wl,-rpath=$(GTILIB_PATH)
	LIBS += -l:libftd3xx.so -Wl,-rpath=$(LIBFTDI_PATH)
else
	LIBS = -ldl -lpthread
	LIBS += -lGTILibrary -Wl,-rpath=$(GTILIB_PATH)
	LIBS += -l:libftd3xx.so -Wl,-rpath=$(LIBFTDI_PATH)
endif

# define for litesample, face sample and jni Android sample 
LIBS_LITE =$(LIBS) 
LIBS_JNI  =$(LIBS) 
#LIBS_ALL += -lopencv_world -ljpeg -lpng -ltiff -llzma -lz

# define the C source files
LITESRCS = liteSample.cpp Classify.cpp
JNISRCS  = jniSample.cpp Classify.cpp

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
LITEOBJS = $(LITESRCS:%.cpp=%.o)
JNIOBJS  = $(JNISRCS:%.cpp=%.o)
LITEDEPS = $(LITEOBJS:%.o=%.d)
JNIDEPS  = $(JNIOBJS:%.o=%.d)

# define the executable file 
LITEMAIN = liteSample
JNIMAIN  = jniSample

# define the static library file 
#MAIN = GTILibrary.a

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all    :  $(LITEMAIN) 
lite   : $(LITEMAIN)
jni    : $(JNIMAIN)

classifylib : libGtiClassify.so

$(LITEMAIN) : $(LITEOBJS)
	$(GTI_CC) $(CFLAGS) $(LFLAGS) $^ -o $@ $(LIBS_LITE)

$(JNIMAIN) : $(JNIOBJS)
	$(GTI_CC) $(CFLAGS) $(LFLAGS) $^ -o $@ $(LIBS_JNI)


libGtiClassify.so: Classify.o
	$(GTI_CC) -shared -Wl,-soname,$@ -o $(GTILIB_PATH)/$@ $^

-include  $(LITEDEPS) $(JNIDEPS)

%.o: %.cpp
	@$(GTI_CC) $(CFLAGS) $(INCLUDES) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(GTI_CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM)   $(LITEOBJS) $(LITEDEPS)    $(JNIOBJS) $(JNIDEPS) *~  $(LITEMAIN)  $(JNIMAIN)


cleanlite:
	$(RM) $(LITEOBJS) $(LITEDEPS) *~ $(LITEMAIN)

cleanjni:
	$(RM) $(JNIOBJS) $(JNIDEPS) *~ $(JNIMAIN)

cleanclassify:
	$(RM) $(GTILIB_PATH)/libGtiClassify.so

#depend: $(SRCS)
#	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

