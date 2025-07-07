#!/bin/sh
PROG="./stitchgraph samples/round samples/shortrows samples/shorttun"
valgrind --tool=massif --massif-out-file=massif.out $PROG
#valgrind --tool=dhat --dhat-out-file=dhat.out $PROG
ms_print massif.out | less
