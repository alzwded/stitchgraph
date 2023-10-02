#!/bin/sh

fail() {
    echo ${1-failed}
    exit 2
}

make || fail 'failed to build'

rm -f samples/README.md
touch samples/README.md

for i in samples/*.png ; do
    ./stitchgraph ${i%.*} || fail $i
    ii=${i##*/}
    title="# ${ii%.*}"
    title=$( echo $title | sed -e 's/_/\\_/g' )
    echo "${title}" >> samples/README.md
    echo >> samples/README.md
    echo '```' >> samples/README.md
    cat ${i%.*} >> samples/README.md
    echo '```' >> samples/README.md
    echo >> samples/README.md
    alt_text=$( echo $i | sed -e 's/_/\\_/g' )
    echo "![${alt_text}](${ii})" >> samples/README.md
    echo >> samples/README.md
done
