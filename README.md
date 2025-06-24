Jak's StitchGraph
=================

This is a very basic tool to help visualize complex patterns and to check stitch counts and shaping is as expected.

It mostly renders as-if you drew the pattern out on graph paper (in full), but it does also try to adjust the work as if stitches are being pulled left or right (but not vertically, because it would be hard to keep track of rows).

The way it works is you give it stitching instructions and it follows them.

Here's an attention grabbing image:

![nutests/edge.png](edge.png)

See [samples](./samples/README.md).

Gawrsh, I hope the above image isn't broken because that would be embarrassing!

Building and Running
====================

Building
--------

If you've spotted the Makefile already and you know what that is, you can skip this section.

This program requires a basic POSIX (=Linux, WSL, maybe MacOS) system with a C++20 compiler (=gcc), GNU make and libpng.

On Windows, please enable WSL with e.g. Ubuntu. (google what I just said) You can then follow the instructions in the "Linux" program you just installed.

Normally you can get everything from your distro (e.g. `apt install gcc g++ make libgpng-dev` or similar);

After that, run `make` in the root directory of the thing you got (via Git or download-as-zip).

Running
-------

`./stitchgraph pattern_file` will produce `pattern_file.png` which you can ogle at.

What's a *pattern file*? Read below.

File format
===========

Comments start with a `#`.

    # This is a comment

Rows may be labelled as the first thing on a line, followed by `:`. If the label is a number, it resets the row count from that number.

    foundation:

    27:

Stitching instructions are formed of a stitch name followed by an optional count.

    k2tog
    k 47

Stitching instructions may be grouped with `*`s or parentheses followed by a mandatory repeat count. If I remember correctly, they may be nested.

    * k yo SSK k k2tog yo * 4

Putting it all together:

    3: pu k    * k yo SSK k k2tog yo * 4   k 2   turn # right side
    4: p 28 turn

There are some special "stitches" to be aware of.

See [samples](./samples/README.md) or [nutests](./nutests).

Special "stitches"
------------------

There are "bind off" stitches which will no longer be counted, and you can't "pick them up" anymore. Examples are `bo` (renders) and `sk` (doesn't render).

There is the "broken" (`/`) stitch. All this does is inform the program that the following stitch is not connected to the previous stitch (e.g. to account for working off two yarn skeins, or switching from in-the-round to flat in Tunisian etc). You do need to remember to place this whenever this happens. The layouting engine also takes these into account, and it groups continuous stitches for resolving tension.

Another one is the "slip" stitch. These get added automatically by the parser when your rows are short (in either direction). You can place these manually as well. Unlike the `pu` stitch which gets a fancy render, these are only ever rendered once you come back to them (and you will see connecting lines stretching downards across multiple rows). This one probably requires more work as I haven't tested it in complex patterns whicoh have wildly varying widths and heights.

Finally, there are some keywords that control how the program works which are not implemented as "stitches" (for reasons I hope are obvious).

There is a "marker" (`!`) which can have letters glued to it (e.g. `!pm`, notice there are no spaces). This will just render as a red `!` at that position.

These are the available end-of-row instructions:

- `turn`, which will continue the following row in the opposite direction;
- `return` and `j`, which will continue the next row in the same direction. There is no functional difference between `return` and `j` (join), it's more for readability :-)

How charts are built
--------------------

The program knows enough about knitting (normal and tunisian) to be able to follow simple instructions. It starts the first row left-to-right (it assumes it's a cast-on or chain N row and it expects you will "turn".

It first parses your input. It does *some* validation (did you pick up more stitches than there are? error. Did you not stitch enough? It slips the remaining stitches in case you need them later). But it might just crash without a good error message :-)

After that, it expands the stitching instructions into "dots" (or "knots"). This is akin to spelling out a full chart on graph paper, placing (or "putting") a dot for each stitch. At this stage it deals with creating connections to neighbouring stitches. It always tries to connect to the previous stitch on the same row (unless a `/` break is present), and it then "takes" stitches from the previous row and connects them to the newly formed stitches per the internal stitch database. Bound off stitches will not get picked up ever again.

After everything is connected, it then proceeds to align the rows of dots in a somewhat natural way (but only horizontally). It takes stitches which are horizontally connected, and centers them over the stitches they are connected to on the row below. It does this for each broken off group (see `/`).

After that, it counts your rows and counts your stitches.

At the end, it begins drawing everything and producint a *PNG* image of the result. Kinda hard to read.

The location of the row number indicates which direction you're supposed to be reading the chart in. If it's on the right, it goes right to left.

How are stitch counts calculated? I don't even know anymore. The intention was "how many stitches you have on your needle at the end of the row", but I need to do another pass to check that's true. Also, that metric doesn't really work if you're thinking in terms of "crochet".

Stitches
========

See [stitches.h](./stitches.h). Or [Stitches.md](./Stitches.md)

The information that goes into a stitch is as follows:

- `key` (used in stitching instructions by the parser)
- `description`
- what kind of special stitch it is (`P` = slipped, `BINDOFF`, `BREAK`) or isn't (`N`)
- `takes` is how many stitches from the row below are worked
- `puts` are how many new stitches are produced
- `marker` is a symbol to place where the knot (dot) should be
- `color` is the color of the symbol
- `map` contains a map of what lines to draw when connecting stitches to the row below.

A map entry has a `source` dot (stitch on row below) and a `destination` dot (one of the newly formed stitches). They optionally have a `marker` (e.g. to fake a Tfs). The special value `INBETWEEN` is used to visually represent Knitting's `m1l` and `m1r` which don't actually `take` a stitch, they get formed by picking up from inbetween the stitches. Don't worry about this one.

Connecting line colors have some sort of meaning to them:
- black: normal
- blue: this ends up "behind" in some sense
- green: this is a newly made stitch
- red: something particularily odd

Marker colors also try to have some meaning:
- black, blue: normal
- green: newly generated stitches (e.g. chains, cast ons)
- red: decrease of some sort *or* something attention grabbing
