#!/bin/sh

fail() {
    echo ${1-failed}
    exit 2
}

make || fail 'failed to build'

rm -f samples/README.md
touch samples/README.md

for i in samples/*.png ; do
    ./stitchgraph < ${i%.*}
    cp graph.png $i
    ii=${i##*/}
    title="# ${ii%.*}"
    title=${title//_/\\_}
    echo "${title}" >> samples/README.md
    echo >> samples/README.md
    echo '```' >> samples/README.md
    cat ${i%.*} >> samples/README.md
    echo '```' >> samples/README.md
    echo >> samples/README.md
    echo "![${i//_/\\_}](${ii})" >> samples/README.md
    echo >> samples/README.md
done
