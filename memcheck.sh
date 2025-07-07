#!/bin/sh
set -e
echo "rebuilding in debug mode"
make clean
make CXXFLAGS=-g
PROG="./stitchgraph samples/round samples/shortrows samples/shorttun"
valgrind --tool=memcheck --leak-check=full --errors-for-leak-kinds=definite --show-leak-kinds=definite --track-origins=yes --error-exitcode=1 $PROG
