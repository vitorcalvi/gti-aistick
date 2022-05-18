
GTI_CC ?= g++
CPP_FLAGS = -O0 -Wall -Wextra
INCLUDE = -I./include
USBLIB_FLAG = -lusb-1.0
TARGET = usbTool

all: $(wildcard *.cpp) $(wildcard *.hpp)
	$(GTI_CC) $(CPP_FLAGS) -o $(TARGET) usbTool.cpp -std=c++11 $(INCLUDE) $(USBLIB_FLAG)

clean:
	rm $(TARGET)

