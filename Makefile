CXXFLAGS ?= -g
CXXFLAGS += -I. --std=c++17

OBJS = main.o

stitchgraph: $(OBJS)
	c++ -o $@ $(OBJS)

%.o: %.cpp
	c++ -c $(CXXFLAGS) -o $@ $<

main.cpp: font.h stitches.h

clean:
	rm -rf *.o stitchgraph
