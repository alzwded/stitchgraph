# StitchGraph examples

This directory contains samples of what StitchGraph can do.

See [the main README.md](../README.md) for general information.

See [Stitches.md](../Stitches.md) for stitches available in this version.

# ball

```
# Basic crochet ball

ch 1 turn
1: 6s 1 join
2: 2s 6 join
3: * 2s 1  s 1 * 6 join
4: * 2s 1  s 2 * 6 join
5: * 2s 1  s 3 * 6 join
6: * 2s 1  s 4 * 6 join

7: s 36 join
8: s 36 join
9: s 36 join
10: s 36 join

11: * s2tog 1 s 4 * 6 join
12: * s2tog 1 s 3 * 6 join
13: * s2tog 1 s 2 * 6 join
14: * s2tog 1 s 1 * 6 join
15: s2tog  6 join
16: s2tog 3 join

16: bo 3
```

![samples/ball.png](ball.png)

# blue

```
co 13 turn
p 13 turn
k 1 k2tog 1 yo 1 k 1 yo 1 SSK 1 k 1 k2tog 1 yo 1 k 1 yo 1 SSK 1 k 1 turn
p 13 turn
k2tog 1 yo 1 k 3 yo 1 SK2P 1 yo 1 k 3 yo 1 SSK 1 turn
p 13 turn
k 1 yo 1 SSK 1 k 1 k2tog 1 yo 1 k 1 yo 1 SSK 1 k 1 k2tog 1 yo 1 k 1 turn
p 13 turn
k 2 yo 1 SK2P 1 yo 1 k 3 yo 1 SK2P 1 yo 1 k 2 turn

!garter k 13 turn
k 13 turn
k 13 turn
k 13 turn
k 13 turn
k 13 turn
k 6 kfb 1 k 6 turn
```

![samples/blue.png](blue.png)

# edge

```
# Edge lace

co 8 turn
1: pu 1 k 2 yo 1 k2tog 1 k 1 yo 2 k 2 turn
2: pu 1 k 2 p 1 k 3 yo 1 k2tog 1 k 1 turn
3: pu 1 k 2 yo 1 k2tog 1 k 5 turn
4: pu 1 k 6 yo 1 k2tog 1 k 1 turn
5: pu 1 k 2 yo 1 k2tog 1 k 1 yo 2 k2tog 1 yo 2 k 2 turn
6: pu 1 k 2 p 1 k 2 p 1 k 3 yo 1 k2tog 1 k 1 turn
7: pu 1 k 2 yo 1 k2tog 1 k 8 turn
8: bo 5 pu 1 k 4 yo 1 k2tog 1 k 1 turn

second rep:
1: pu 1 k 2 yo 1 k2tog 1 k 1 yo 2 k 2 turn
2: pu 1 k 2 p 1 k 3 yo 1 k2tog 1 k 1 turn
3: pu 1 k 2 yo 1 k2tog 1 k 5 turn
4: pu 1 k 6 yo 1 k2tog 1 k 1 turn
5: pu 1 k 2 yo 1 k2tog 1 k 1 yo 2 k2tog 1 yo 2 k 2 turn
6: pu 1 k 2 p 1 k 2 p 1 k 3 yo 1 k2tog 1 k 1 turn
7: pu 1 k 2 yo 1 k2tog 1 k 8 turn
8: bo 5 pu 1 k 4 yo 1 k2tog 1 k 1 turn

third rep:
1: pu 1 k 2 yo 1 k2tog 1 k 1 yo 2 k 2 turn
2: pu 1 k 2 p 1 k 3 yo 1 k2tog 1 k 1 turn
3: pu 1 k 2 yo 1 k2tog 1 k 5 turn
4: pu 1 k 6 yo 1 k2tog 1 k 1 turn
5: pu 1 k 2 yo 1 k2tog 1 k 1 yo 2 k2tog 1 yo 2 k 2 turn
6: pu 1 k 2 p 1 k 2 p 1 k 3 yo 1 k2tog 1 k 1 turn
7: pu 1 k 2 yo 1 k2tog 1 k 8 turn
8: bo 5 pu 1 k 4 yo 1 k2tog 1 k 1 turn

bo 8
```

![samples/edge.png](edge.png)

# fan\_and\_split

```
# test out the `break` stitch, e.g. to split off two shoulder halfs or v-neck

co 10 turn
k 10 turn
p 1 * yo 1 p 1 * 9 turn
k 19 turn
p 19 turn
k 4 / bo 11 k 4 turn
p 4 / p 4 turn
k 1 k2tog 1 k 1 / k 1 SSK 1 k 1 turn
p 1 p2tog 1 / SSP 1 p 1 turn
k2tog 1 / SSK 1 turn
bo 1 / bo 1
```

![samples/fan\_and\_split.png](fan_and_split.png)

# incs

```
# test out increases

co 8 turn
1: k 8 turn
2: p 8 turn
3: pu 1 k 1   k 1 kfb   k 1 kbf  k 2 turn
4: p 10 turn
5: pu 1 * k 1 k2tog 1 k 1 * 2 k 1 turn
6: p 8 !warn turn

7: k 3 M 1 k 2 M 1 k 3 return
8: k 3 M 1 k 4 M 1 k 3 return
9: k 2 CDD 1 k 2 CDD 1 k 2 return

10: k 2 kll k 2 krl k 2 turn
11: p 10 turn
12: k 2 SSK k 2 k2tog k 2 turn
13: p 8 turn
14: k 3 m1l k 2 m1r k 3 turn
15: p 10 turn
16: k 2 SK2P SK2P k 2 turn

bo 6
```

![samples/incs.png](incs.png)

# incs\_simple

```
# test out increases

co 8 turn
1: k 8 turn
2: p 8 turn
3: pu 1 k 1 * k 1 M 1 k 1 * 2 k 2 turn
4: p 10 turn
5: pu 1 * k 1 k2tog 1 k 1 * 2 k 1 turn
6: p 8 !warn turn

7: k 3 M 1 k 2 M 1 k 3 return
8: k 3 M 1 k 4 M 1 k 3 return
9: k 2 CDD 1 k 2 CDD 1 k 2 return

bo 8
```

![samples/incs\_simple.png](incs_simple.png)

# kirby

```
# Kirby Amigurumi :-)

# body
ch 1 turn
1: 6s join
2: * 2s * 6 join
3: * s 2s * 6 join
4: * s 2 2s * 6 join
5: * s 3 2s * 6 join
6: * s 4 2s * 6 join

7: s 36 join
8: s 36 join
9: s 36 join
10: s 36 join
11: s 36 join

12: * s2tog s 4 * 6 join
13: * s2tog s 3 * 6 join
14: * s2tog s 2 * 6 join
15: * s2tog s 1 * 6 join
16: * s2tog * 6 join

0: sk 6 join

# arm, mk 2
0: ch 1
1: 5s join
2: * 2s * 5 join
3: * s 2s * 5 join
4: s 15 join
5: s 15 join

0: sk 15 join

# leg, mk 2
0: ch 1
1: 3s join
2: 2s 3 join
3: * s 2s * 3 join
4: * s 2 2s * 3 join
5: * s 3 2s * 3 join
6: s 15 join
7: s 15 join
8: s 15 join
9: s 15 join
10: s 15 join
11: * s2tog s * 5 join
12: s2tog 5 join

# embroider eyes and mouth or whatnot
```

![samples/kirby.png](kirby.png)

# lace

```
co 14 turn
k 14 turn
p 14 turn
k 1 * yo 1 k2tog 1 k 1 SSK 1 yo 1 k 1 * 2 k 1 turn
p 14 turn
k 1 * SSK 1 yo 1 k 1 yo 1 k2tog 1 k 1 * 2 k 1 turn
p 14 turn
k 14 turn
bo 14
```

![samples/lace.png](lace.png)

# markers\_simple

```
# test out that markers get placed where they should

co 8 turn
pu 1 k 1 !B k 4 !A k 2 turn
pu 1 p 1 !b p 4 !A p 2 !pattern-change turn
!pattern-change pu 1 * p 1 k 1 * 3 p 1 turn
pu 1 * p 1 k 1 * 3 p 1 turn
```

![samples/markers\_simple.png](markers_simple.png)

# round

```
# test out a big ball with a vertical slit in the middle, requiring a sudden change in direction

   ch 7 !air ch 3 !join-with-self ch 6 join
1: 3s 1 s 6 sk 1 3s 1 sk 1 s 6 join
2: [ 2s 3 s 6 ] 2 join
3: [ * 2s 1 s 1 * 3 s 6 ] 2 join
4: [ * 2s 1 s 2 * 3 s 6 ] 2 join
5: s 36 join
6: s 18 !turn  sk 2 / !return s 16 turn
7: s 16 !turn  /  !return s 18 turn
8: s 18 ch 2 s 16 join
9: s 18  s 18 join
10: * s2tog 1 s 2 * 3 s 6    * s2tog 1 s 2 * 3 s 6 join
```

![samples/round.png](round.png)

# shortrows

```
# test out knit short rows

co 8 turn
pu  k  !B k 4 !A k 2 turn
pu  p  !b p 4 !A p 2 !pattern-change turn
!pattern-change pu 1 * p 1 k 1 * 3 p 1 turn
pu 1 * p 1 k 1 * 3 p 1 turn

# begin short rows
10: k 6 turn
    p 4 turn
    k 6 turn
    p 8 turn

# bind offs?
20: bo 2 k 6 turn
    p 6 turn
    bo 2 pu k 3 turn
    pu p 3 turn
    pu k 3 turn
    bo 2 pu p  turn
    k2tog turn
    bo
```

![samples/shortrows.png](shortrows.png)

# shorttun

```
# Short Rows when using return/join

# round / tunisian
30: co 8 turn
# short left
    Tss 8 return
    Tss 6 return
    Tss 4 return
    Tss 2 return
    Tss 8 return
# short right; requires explicit "blanks" to get it to look like this;
41: - 2 Tss 6 return
    - 4 Tss 4 return
    - 6 Tss 2 return
    Tss 8 return
# short right; alternatively use `pu' stitch to mark them
# red and explicitly "slipped", although this implies you
# "return" over them... it's up to the beholder, we can do both
51: pu 2 Tss 6 return
    pu 4 Tss 4 return
    pu 6 Tss 2 return
    Tss 8 return
# alternating bind offs
61: bo 2 Tss 6 return
    Tss 4 return
    bo Tss 2 return
    bo 5
```

![samples/shorttun.png](shorttun.png)

# simplest

```
co 28 turn
k 28 turn
p 28 turn # wrong side
3: pu k    !pm  * k yo SSK k k2tog yo * 4  !pm    k 2   turn # right side
4: p 28 turn
```

![samples/simplest.png](simplest.png)

# tfs

```
# test rendering of glyphs on lines

ch 8 turn
| Tss 6 || return
| Tfs 6 || return
| Tfs 6 || return
| Tfs 6 || return
bo 8
```

![samples/tfs.png](tfs.png)

# tunisianx

```
# test out the break stitch in a tunisian context and implied return/join

co 10 turn
 1: | 10 return
 2: | 1 Xss 4 || 1 return
 3: | 1 Tss 1 Xss 3 Tss 1 || 1 return
 4: | 1 Xss 4 || 1 return
 5: | 1 Tss 1 Xss 3 Tss 1 || 1 return
 6: | 1 Xss 4 || 1
 7: | 1 Tss 1 Xss 3 Tss 1 || 1
 8: | 1 Xss 4 || 1
 9: | 1 Tss 1 Xss 3 Tss 1 || 1
 # split off, join new yarn
10: | 1 Tss 1 || 1 ! / bo 4       | 1  Tss 1 || 1
11: | 1 Tss 1 || 1   /            | 1  Tss 1 || 1
12: bo 3             /            bo 3
```

![samples/tunisianx.png](tunisianx.png)

