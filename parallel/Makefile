# --------------------------------------------------------
# Video re - targeting in parallel
# --------------------------------------------------------

# You have to set the following 2 variable:
# FF_ROOT     pointing to the FastFlow root directory (i.e.
#             the one containing the ff directory).

FF_ROOT	= $(HOME)/fastflow
#FF_ROOT	= $(HOME)/project/fastflow-2.1.0

INCS = -I $(FF_ROOT)

OBJS = main.o video.o pcarving.o psobel.o

CXX	= g++ -std=c++11 -O3 $(INCS)
#CXX	= icc -std=c++11 $(INCS)
CFLAGS 	= -Wall `pkg-config --cflags opencv` #-DTRACE_FASTFLOW  -pg
OPT     = -O3 -finline-functions -DNDEBUG
LIBS 	= `pkg-config --libs opencv` -pthread

video :	$(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) $(OPT) -o video $(LIBS)

seam: pcarving.o psobel.o
	$(CXX) $(CFLAGS) $(OPT) pcarving.o psobel.o -o seam $(LIBS)

clean:
	-rm *.o video
