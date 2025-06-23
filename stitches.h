#ifndef STITCHES_H
#define STITCHES_H

#include "common.h"
#include <vector>

struct MapEntry
{
    enum SPECIAL_SRC_VALUES {
        INBETWEEN = -1
    };
    int src = 0;
    int dst = 0;
    Color color = BLACK;
    Marker marker = NONE;
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

    bool IsSlipped() { return strcmp(key, "-") == 0; }
    bool IsBroken() { return strcmp(key, "/") == 0; }
};

static std::vector<Stitch> STITCHES {
    // BLUE vertical stitches are below
#define BLANK_STITCH_INDEX 0
        {
            "-", "(blank)", true,
            1, 1,
        },
#define BREAK_STITCH_INDEX 1
        {
            "/", "(break; unconnected to previous stitch)", true,
            0, 0,
        },
        {
            "pu", "pick up / slip", false,
            1, 1,
            NONE, RED,
            {
                { 0, 0, RED },
            }
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
                { 0, 0, GREEN },
                { 0, 1, BLACK },
            }
        },
        {
            "M", "make one (KFB)", false,
            1, 2,
            ARROWDOWN, GREEN,
            {
                { 0, 0, GREEN },
                { 0, 1, BLACK },
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
        /*
        {
            "Tfs", "Tunisian full stitch", false,
            1, 1,
            DOT, BLACK,
            {
                { MapEntry::INBETWEEN, 0, BLACK, CIRCLE },
            }
        },
        */
        {
            "Tfs", "Tunisian full stitch", false,
            1, 1,
            DOT, BLACK,
            {
                { 0, 0, BLACK, CIRCLE },
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
            ARROWLEFT, GREEN,
            {
                { MapEntry::INBETWEEN, 0, BLACK }
            }
        },
        {
            "ma", "make 1 air", false,
            0, 1,
            ARROWDOWN, GREEN,
            {
                { 0, 0, GREEN }
            }
        },
        {
            "mr", "make 1 right", false,
            0, 1,
            ARROWRIGHT, GREEN,
            {
                { MapEntry::INBETWEEN, 0, BLUE }
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
            "SK2P", "sl k2tog psso", false,
            3, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLACK },
                { 1, 0, BLUE },
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
        {
            "dc", "double crochet", false,
            1, 1,
            DOT, BLACK,
            {
                { 0, 0, BLACK, HBAR },
            }
        },
        {
            "tc", "triple crochet", false,
            1, 1,
            DOT, BLACK,
            {
                { 0, 0, BLACK, H2BAR },
            }
        },
        {
            "qc", "quadruple crochet", false,
            1, 1,
            DOT, BLACK,
            {
                { 0, 0, BLACK, H3BAR },
            }
        },
        {
            "bb", "bobble", false,
            1, 1,
            DOT, RED,
            {
                { 0, 0, RED, BOBBLE },
            }
        },
        {
            "c2f", "cable 2 front", false,
            3, 3,
            DOT, BLACK,
            {
                { 0, 1, BLACK },
                { 1, 2, BLACK },
                { 2, 0, BLUE },
            }
        },
        {
            "c2b", "cable 2 back", false,
            3, 3,
            DOT, BLUE,
            {
                { 0, 2, BLUE },
                { 1, 0, BLACK },
                { 2, 1, BLACK },
            }
        },
        {
            "c3b", "cable 3 back", false,
            6, 6,
            DOT, BLUE,
            {
                { 0, 3, BLUE },
                { 1, 4, BLUE },
                { 2, 5, BLUE },
                { 3, 0, BLACK },
                { 4, 1, BLACK },
                { 5, 2, BLACK },
            }
        },
        {
            "c3f", "cable 3 front", false,
            6, 6,
            DOT, BLACK,
            {
                { 0, 3, BLACK },
                { 1, 4, BLACK },
                { 2, 5, BLACK },
                { 3, 0, BLUE },
                { 4, 1, BLUE },
                { 5, 2, BLUE },
            }
        },
        {
            "c3b", "cable 3 back", false,
            6, 6,
            DOT, BLUE,
            {
                { 0, 3, BLUE },
                { 1, 4, BLUE },
                { 2, 5, BLUE },
                { 3, 0, BLACK },
                { 4, 1, BLACK },
                { 5, 2, BLACK },
            }
        },
};

#endif
