CXXFLAGS ?= -g
CXXFLAGS += -I. --std=c++17

OBJS = main.o draw.o

stitchgraph: $(OBJS)
	c++ -o $@ $(OBJS)

%.o: %.cpp
	c++ -c $(CXXFLAGS) -o $@ $<

main.cpp: stitches.h draw.h common.h

draw.cpp: draw.h common.h

clean:
	rm -rf *.o stitchgraph
