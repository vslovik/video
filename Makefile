TARGET ?= video
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -std=c++11
LFLAGS = `pkg-config opencv --cflags --libs` -pthread

OBJ = $(addsuffix .o, $(basename $(SRC)))

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(SRCS) -o $@ $(LFLAGS)