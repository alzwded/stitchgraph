#!/bin/sh

fail() {
    echo "${1-failed}"
    touch "$1.fail"
    if [ ${2-0} -gt 0 ] ; then
        exit $2
    fi
}

make || fail 'failed to build' 2

rm -f samples/README.md
touch samples/README.md

rm -f samples/*.fail

for i in samples/*.png ; do
    ./stitchgraph "${i%.*}" || fail "$i failed"
    ii=${i##*/}
    title="# ${ii%.*}"
    title=$( echo "$title" | sed -e 's/_/\\_/g' )
    {
        echo "${title}"
        echo ''
        echo '```'
        cat "${i%.*}"
        echo '```'
        echo ''
        alt_text=$( echo "$i" | sed -e 's/_/\\_/g' )
        echo "![${alt_text}](${ii})"
        echo ''
    } >> samples/README.md
done

ls samples/*.fail
