CXXFLAGS ?= -g
CXXFLAGS += -I. --std=c++20 ` pkg-config --cflags libpng `

OBJS = main.o draw.o

all: stitchgraph Stitches.md

stitchgraph: $(OBJS)
	c++ -o $@ $(OBJS) ` pkg-config --libs libpng `

Stitches.md: build_stitches_md
	./build_stitches_md

build_stitches_md: build_stitches_md.o
	c++ -o $@ build_stitches_md.o

%.o: %.cpp
	c++ -c $(CXXFLAGS) -o $@ $<

main.o: stitches.h draw.h common.h main.cpp

draw.o: draw.h common.h draw.cpp

build_stitches_md.o: stitches.h build_stitches_md.cpp

clean:
	rm -rf *.o stitchgraph build_stitches_md
