CXXFLAGS ?= -g
CXXFLAGS += -I. --std=c++20 ` pkg-config --cflags libpng `

OBJS = main.o draw.o

stitchgraph: $(OBJS)
	c++ -o $@ $(OBJS) ` pkg-config --libs libpng `

%.o: %.cpp
	c++ -c $(CXXFLAGS) -o $@ $<

main.o: stitches.h draw.h common.h

draw.o: draw.h common.h

clean:
	rm -rf *.o stitchgraph
