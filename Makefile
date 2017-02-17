TARGET ?= video
SRC_DIRS ?= ./src
FF_DIR ?= /home/valeriya/project/fastflow-2.1.0

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -std=c++11 -I $(FF_DIR) -lpthread
LFLAGS = `pkg-config opencv --cflags --libs` -pthread

OBJ = $(addsuffix .o, $(basename $(SRC)))

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(SRCS) -o $@ $(LFLAGS)