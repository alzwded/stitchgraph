#ifndef STITCHES_H
#define STITCHES_H

#include "common.h"
#include <vector>

struct MapEntry
{
    int src;
    int dst;
    Color color;
};

struct Stitch
{
    const char* key = "";
    const char* description = "";
    bool blank = true;
    int takes = 0;
    int puts = 0;
    Marker marker = DOT;
    Color color = BLACK;
    std::vector<MapEntry> map = {};
    bool markerBefore = false;
    bool markerAfter = false;
};

static std::vector<Stitch> STITCHES {
    // BLUE vertical stitches are below
        {
            "-", "(blank)", true,
            1, 1,
        },
        {
            "<", "(blank, pad left)", true,
            0, 0,
        },
        {
            ">", "(blank, pad right)", true,
            0, 0,
        },
        {
            "co", "cast on", false,
            0, 1,
            DOT, GREEN,
            {}
        },
        {
            "bo", "bind off", true,
            1, 1,
        },
        {
            "sk", "skip", true,
            1, 0,
        },
        {
            "k", "knit", false,
            1, 1,
            DOT, BLACK,
            { 
                { 0, 0, BLACK },
            }
        },
        {
            "p", "purl", false,
            1, 1,
            SQUIGGLE, BLUE,
            {
                { 0, 0, BLACK },
            }
        },
        {
            "k2tog", "knit two together", false,
            2, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLUE },
                { 1, 0, BLACK },
            }
        },
        {
            "SSK", "slip slip knit", false,
            2, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLACK },
                { 1, 0, BLUE },
            }
        },
        {
            "p2tog", "purl two together", false,
            2, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLACK },
                { 1, 0, BLUE },
            }
        },
        {
            "SSP", "slip slip purl", false,
            2, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLUE },
                { 1, 0, BLACK },
            }
        },
        {
            "Tss", "tunisian simple stitch", false,
            1, 1,
            DOT, BLACK,
            {
                { 0, 0, BLACK },
            }
        },
        {
            "kfb", "knit front and back", false,
            1, 2,
            ARROWDOWN, GREEN,
            {
                { 0, 0, BLACK },
                { 0, 1, GREEN },
            }
        },
        {
            "pu", "pick up", false,
            1, 1,
            DOT, RED,
            {
                { 0, 0, RED },
            }
        },
        {
            "|", "Tunisian pick up", false,
            1, 1,
            DOT, RED,
            {
                { 0, 0, RED },
            }
        },
        {
            "||", "Tunisian end of row", false,
            1, 1,
            DOT, RED,
            {
                { 0, 0, GREEN },
            }
        },
        {
            "Xss", "crossed tunisian simple stitch", false,
            2, 2,
            DOT, BLUE,
            {
                { 0, 1, BLACK },
                { 1, 0, BLUE },
            }
        },
        {
            "yo", "yarn over", false,
            0, 1,
            CIRCLE, GREEN,
        },
        {
            "ml", "make 1 left", false,
            0, 1,
            ARROWDOWN, GREEN,
            {
                { -1, 0, BLACK }
            }
        },
        {
            "M", "make 1", false,
            0, 1,
            ARROWDOWN, GREEN,
            {
                { 0, 0, BLACK }
            }
        },
        {
            "mr", "make 1 right", false,
            0, 1,
            ARROWDOWN, GREEN,
            {
                { +1, 0, BLACK }
            }
        },
        {
            "CDD", "central double decrease", false,
            3, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLUE },
                { 1, 0, BLACK },
                { 2, 0, BLUE },
            }
        },
        {
            "s", "generic stitch", false,
            1, 1,
            DOT, BLACK,
            {
                { 0, 0, BLACK },
            }
        },
        {
            "2s", "generic stitch twice in same loop", false,
            1, 2,
            ARROWDOWN, GREEN,
            {
                { 0, 0, BLACK },
                { 0, 1, GREEN }
            }
        },
        {
            "3s", "generic stitch thrice in same loop", false,
            1, 3,
            ARROWDOWN, GREEN,
            {
                { 0, 0, BLACK },
                { 0, 1, GREEN },
                { 0, 2, GREEN }
            }
        },
        {
            "4s", "generic stitch 4 in same loop", false,
            1, 4,
            ARROWDOWN, GREEN,
            {
                { 0, 0, BLACK },
                { 0, 1, GREEN },
                { 0, 2, GREEN },
                { 0, 2, GREEN },
            }
        },
        {
            "5s", "generic stitch 5 in same loop", false,
            1, 5,
            ARROWDOWN, GREEN,
            {
                { 0, 0, BLACK },
                { 0, 1, GREEN },
                { 0, 2, GREEN },
                { 0, 3, GREEN },
                { 0, 4, GREEN },
            }
        },
        {
            "6s", "generic stitch 6 in same loop", false,
            1, 6,
            ARROWDOWN, GREEN,
            {
                { 0, 0, BLACK },
                { 0, 1, GREEN },
                { 0, 2, GREEN },
                { 0, 3, GREEN },
                { 0, 4, GREEN },
                { 0, 5, GREEN },
            }
        },
        {
            "7s", "generic stitch 7 in same loop", false,
            1, 7,
            ARROWDOWN, GREEN,
            {
                { 0, 0, BLACK },
                { 0, 1, GREEN },
                { 0, 2, GREEN },
                { 0, 3, GREEN },
                { 0, 4, GREEN },
                { 0, 5, GREEN },
                { 0, 6, GREEN },
            }
        },
        {
            "8s", "generic stitch 8 in same loop", false,
            1, 8,
            ARROWDOWN, GREEN,
            {
                { 0, 0, BLACK },
                { 0, 1, GREEN },
                { 0, 2, GREEN },
                { 0, 3, GREEN },
                { 0, 4, GREEN },
                { 0, 5, GREEN },
                { 0, 6, GREEN },
                { 0, 7, GREEN },
            }
        },
        {
            "s2tog", "generic stitch two together", false,
            2, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLUE },
                { 1, 0, BLACK }
            }
        },
        {
            "s3tog", "generic stitch three together", false,
            3, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLUE },
                { 1, 0, BLUE },
                { 2, 0, BLACK },
            }
        },
        {
            "ch", "chain", false,
            0, 1,
            CIRCLE, GREEN,
        },
};

#endif
