#!/bin/sh

fail() {
    echo ${1-failed}
    exit 2
}

make || fail 'failed to build'

set -x
for i in samples/*.png ; do
    ./stitchgraph < ${i%.*}
    cp graph.png $i
done
