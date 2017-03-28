#TARGET ?= video
#SRC_DIRS ?= ./src
#FF_DIR ?= /home/valeriya/project/fastflow-2.1.0

#SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
#OBJS := $(addsuffix .o,$(basename $(SRCS)))

#CFLAGS = -std=c++11 -I $(FF_DIR) -lpthread
#LFLAGS = `pkg-config opencv --cflags --libs` -pthread

#OBJ = $(addsuffix .o, $(basename $(SRC)))

#$(TARGET): $(OBJS)
#	$(CXX) $(CFLAGS) $(SRCS) -o $@ $(LFLAGS)

# ---------------------------------------------------------------------------
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  As a special exception, you may use this file as part of a free software
#  library without restriction.  Specifically, if other files instantiate
#  templates or use macros or inline functions from this file, or you compile
#  this file and link it with other files to produce an executable, this
#  file does not by itself cause the resulting executable to be covered by
#  the GNU General Public License.  This exception does not however
#  invalidate any other reasons why the executable file might be covered by
#  the GNU General Public License.
#
# ---------------------------------------------------------------------------

# You have to set at least the following 2 variables:
# OPENCV_ROOT pointing to the OpenCV root directory
# FF_ROOT     pointing to the FastFlow root directory (i.e.
#             the one containing the ff directory).
# OCL_ROOT    pointing to the OpenCL root directory
#
OPENCV_ROOT = /usr/local/include/opencv2
FF_ROOT	= /home/valeriya/project/fastflow-2.1.0

CXX = g++ -std=c++11 -I /home/valeriya/project/fastflow-2.1.0 -I /usr/local/include/opencv2 -L /usr/local/lib/
INCS = -I $(FF_ROOT)

OBJS = main.o video.o pcarving.o psobel.o
DEBUG = -g

CXXFLAGS = `pkg-config --cflags opencv` `pkg-config --libs opencv` -pthread
OPTFLAGS = -O3 -finline-functions -DNDEBUG

#video: $(OBJS)
#	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(OBJS) -o video


CXX	= g++ -std=c++11 -I /home/valeriya/project/fastflow-2.1.0
FFHOME = "/home/valeriya/project/fastflow-2.1.0"
CFLAGS 	= -Wall `pkg-config --cflags opencv` #-DTRACE_FASTFLOW  -pg
OPT     = -O3 -DNDEBUG
LIBS 	= `pkg-config --libs opencv` -pthread

#TARGETS	= video ffvideo ffvideo_nf ffvideo_nf2

#all: $(TARGETS)

video :	$(OBJS)
	$(CXX)  $(CFLAGS) $(OBJS) -o video $(LIBS)

clean:
	-rm *.o video
