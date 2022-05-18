
# Setup gti build environment.
ifndef GTISDKROOT
	include ../../Build/GTISDK.env
endif

GTI_CC ?= g++

TARGET = eusbTool

all: $(wildcard *.cpp) $(wildcard *.hpp)
	$(GTI_CC) -o $(TARGET) -std=c++11 eusbTool.cpp

clean:
	rm $(TARGET)

