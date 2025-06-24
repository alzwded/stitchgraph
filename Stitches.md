## `-`

- Symbol: `-`
- Description: slip/pick up/ignore (special slip/blank stitch)
- takes: 1
- puts:  1
- marker: DOT

## `/`

- Symbol: `/`
- Description: break; unconnected to previous stitch on same row (special broken stitch to allow multiple skeins or creating big holes)
- takes: 0
- puts:  0
- marker: DOT

## `pu`

- Symbol: `pu`
- Description: pick up / slip 
- takes: 1
- puts:  1
- marker: CIRCLE
- map: (0 <- 0) 

## `co`

- Symbol: `co`
- Description: cast on 
- takes: 0
- puts:  1
- marker: DOT

## `bo`

- Symbol: `bo`
- Description: bind off (bind off stitch)
- takes: 1
- puts:  1
- marker: VBAR
- map: (0 <- 0) 

## `sk`

- Symbol: `sk`
- Description: skip (bind off stitch)
- takes: 1
- puts:  0
- marker: VBAR

## `k`

- Symbol: `k`
- Description: knit 
- takes: 1
- puts:  1
- marker: DOT
- map: (0 <- 0) 

## `p`

- Symbol: `p`
- Description: purl 
- takes: 1
- puts:  1
- marker: SQUIGGLE
- map: (0 <- 0) 

## `k2tog`

- Symbol: `k2tog`
- Description: knit two together 
- takes: 2
- puts:  1
- marker: ARROWUP
- map: (0 <- 0) (0 <- 1) 

## `SSK`

- Symbol: `SSK`
- Description: slip slip knit 
- takes: 2
- puts:  1
- marker: ARROWUP
- map: (0 <- 0) (0 <- 1) 

## `p2tog`

- Symbol: `p2tog`
- Description: purl two together 
- takes: 2
- puts:  1
- marker: ARROWUP
- map: (0 <- 0) (0 <- 1) 

## `SSP`

- Symbol: `SSP`
- Description: slip slip purl 
- takes: 2
- puts:  1
- marker: ARROWUP
- map: (0 <- 0) (0 <- 1) 

## `Tss`

- Symbol: `Tss`
- Description: tunisian simple stitch 
- takes: 1
- puts:  1
- marker: DOT
- map: (0 <- 0) 

## `kfb`

- Symbol: `kfb`
- Description: knit front and back 
- takes: 1
- puts:  2
- marker: ARROWDOWN
- map: (0 <- 0) (1 <- 0) 

## `M`

- Symbol: `M`
- Description: make one 
- takes: 0
- puts:  1
- marker: ARROWDOWN
- map: (0 <- -1) 

## `|`

- Symbol: `|`
- Description: Tunisian pick up 
- takes: 1
- puts:  1
- marker: DOT
- map: (0 <- 0) 

## `||`

- Symbol: `||`
- Description: Tunisian end of row 
- takes: 1
- puts:  1
- marker: DOT
- map: (0 <- 0) 

## `Xss`

- Symbol: `Xss`
- Description: crossed tunisian simple stitch 
- takes: 2
- puts:  2
- marker: DOT
- map: (1 <- 0) (0 <- 1) 

## `Tfs`

- Symbol: `Tfs`
- Description: Tunisian full stitch 
- takes: 1
- puts:  1
- marker: DOT
- map: (0 <- 0) 

## `yo`

- Symbol: `yo`
- Description: yarn over 
- takes: 0
- puts:  1
- marker: CIRCLE

## `ml`

- Symbol: `ml`
- Description: make 1 left 
- takes: 0
- puts:  1
- marker: ARROWLEFT
- map: (0 <- -1) 

## `ma`

- Symbol: `ma`
- Description: make 1 air 
- takes: 0
- puts:  1
- marker: ARROWDOWN

## `mr`

- Symbol: `mr`
- Description: make 1 right 
- takes: 0
- puts:  1
- marker: ARROWRIGHT
- map: (0 <- -1) 

## `CDD`

- Symbol: `CDD`
- Description: central double decrease 
- takes: 3
- puts:  1
- marker: ARROWUP
- map: (0 <- 0) (0 <- 1) (0 <- 2) 

## `SK2P`

- Symbol: `SK2P`
- Description: sl k2tog psso 
- takes: 3
- puts:  1
- marker: ARROWUP
- map: (0 <- 0) (0 <- 1) (0 <- 2) 

## `s`

- Symbol: `s`
- Description: generic stitch 
- takes: 1
- puts:  1
- marker: DOT
- map: (0 <- 0) 

## `2s`

- Symbol: `2s`
- Description: generic stitch twice in same loop 
- takes: 1
- puts:  2
- marker: ARROWDOWN
- map: (0 <- 0) (1 <- 0) 

## `3s`

- Symbol: `3s`
- Description: generic stitch thrice in same loop 
- takes: 1
- puts:  3
- marker: ARROWDOWN
- map: (0 <- 0) (1 <- 0) (2 <- 0) 

## `4s`

- Symbol: `4s`
- Description: generic stitch 4 in same loop 
- takes: 1
- puts:  4
- marker: ARROWDOWN
- map: (0 <- 0) (1 <- 0) (2 <- 0) (2 <- 0) 

## `5s`

- Symbol: `5s`
- Description: generic stitch 5 in same loop 
- takes: 1
- puts:  5
- marker: ARROWDOWN
- map: (0 <- 0) (1 <- 0) (2 <- 0) (3 <- 0) (4 <- 0) 

## `6s`

- Symbol: `6s`
- Description: generic stitch 6 in same loop 
- takes: 1
- puts:  6
- marker: ARROWDOWN
- map: (0 <- 0) (1 <- 0) (2 <- 0) (3 <- 0) (4 <- 0) (5 <- 0) 

## `7s`

- Symbol: `7s`
- Description: generic stitch 7 in same loop 
- takes: 1
- puts:  7
- marker: ARROWDOWN
- map: (0 <- 0) (1 <- 0) (2 <- 0) (3 <- 0) (4 <- 0) (5 <- 0) (6 <- 0) 

## `8s`

- Symbol: `8s`
- Description: generic stitch 8 in same loop 
- takes: 1
- puts:  8
- marker: ARROWDOWN
- map: (0 <- 0) (1 <- 0) (2 <- 0) (3 <- 0) (4 <- 0) (5 <- 0) (6 <- 0) (7 <- 0) 

## `s2tog`

- Symbol: `s2tog`
- Description: generic stitch two together 
- takes: 2
- puts:  1
- marker: ARROWUP
- map: (0 <- 0) (0 <- 1) 

## `s3tog`

- Symbol: `s3tog`
- Description: generic stitch three together 
- takes: 3
- puts:  1
- marker: ARROWUP
- map: (0 <- 0) (0 <- 1) (0 <- 2) 

## `ch`

- Symbol: `ch`
- Description: chain 
- takes: 0
- puts:  1
- marker: CIRCLE

## `dc`

- Symbol: `dc`
- Description: double crochet 
- takes: 1
- puts:  1
- marker: DOT
- map: (0 <- 0) 

## `tc`

- Symbol: `tc`
- Description: triple crochet 
- takes: 1
- puts:  1
- marker: DOT
- map: (0 <- 0) 

## `qc`

- Symbol: `qc`
- Description: quadruple crochet 
- takes: 1
- puts:  1
- marker: DOT
- map: (0 <- 0) 

## `bb`

- Symbol: `bb`
- Description: bobble 
- takes: 1
- puts:  1
- marker: DOT
- map: (0 <- 0) 

## `c2f`

- Symbol: `c2f`
- Description: cable 2 front 
- takes: 3
- puts:  3
- marker: DOT
- map: (1 <- 0) (2 <- 1) (0 <- 2) 

## `c2b`

- Symbol: `c2b`
- Description: cable 2 back 
- takes: 3
- puts:  3
- marker: DOT
- map: (2 <- 0) (0 <- 1) (1 <- 2) 

## `c3b`

- Symbol: `c3b`
- Description: cable 3 back 
- takes: 6
- puts:  6
- marker: DOT
- map: (3 <- 0) (4 <- 1) (5 <- 2) (0 <- 3) (1 <- 4) (2 <- 5) 

## `c3f`

- Symbol: `c3f`
- Description: cable 3 front 
- takes: 6
- puts:  6
- marker: DOT
- map: (3 <- 0) (4 <- 1) (5 <- 2) (0 <- 3) (1 <- 4) (2 <- 5) 

## `c3b`

- Symbol: `c3b`
- Description: cable 3 back 
- takes: 6
- puts:  6
- marker: DOT
- map: (3 <- 0) (4 <- 1) (5 <- 2) (0 <- 3) (1 <- 4) (2 <- 5) 

