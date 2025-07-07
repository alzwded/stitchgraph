#!/bin/sh

fail() {
    echo "${1-failed}"
    if [ ${2-0} -gt 0 ] ; then
        # exit entirely
        exit $2
    else
        # mark failure but don't exit
        touch "$1.fail"
    fi
}

make || fail 'failed to build' 2

rm -f samples/README.md
touch samples/README.md

cat <<EOT >samples/README.md
# StitchGraph examples

This directory contains samples of what StitchGraph can do.

See [the main README.md](../README.md) for general information.

See [Stitches.md](../Stitches.md) for stitches available in this version.

EOT

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

ls samples/*.fail && exit 1
exit 0
