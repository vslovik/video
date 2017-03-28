# --------------------------------------------------------
# OpenCV installation test
# --------------------------------------------------------

CXX	= g++ -std=c++11
#CXX	= icc -std=c++11 $(INCS)
CFLAGS 	= -Wall `pkg-config --cflags opencv` #-DTRACE_FASTFLOW  -pg
OPT     = -O3 -finline-functions -DNDEBUG
LIBS 	= `pkg-config --libs opencv` -pthread

main :	main.cpp
	$(CXX) $(CFLAGS) $< -o video $(LIBS)

clean:
	-rm video