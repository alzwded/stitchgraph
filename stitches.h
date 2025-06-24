#ifndef STITCHES_H
#define STITCHES_H

#include "common.h"
#include <vector>

struct MapEntry
{
    enum SPECIAL_SRC_VALUES {
        INBETWEEN = -1
    };
    int src = 0; // stitch on row below
    int dst = 0; // newly put stitch
    Color color = BLACK;
    Marker marker = NONE;
};

struct Stitch
{
    const char* key = "";
    const char* description = "";
    enum SPECIAL {
        N = 0, // this is a normal stitch
        P = 1, // this is a padding stitch (slipped or short rows)
        BINDOFF = 2, // this is a bind off (or skip), no longer countable
        BREAK = 3, // this is a separate section / skein
    } special = N;
    int takes = 0;
    int puts = 0;
    Marker marker = DOT;
    Color color = BLACK;
    std::vector<MapEntry> map = {};
    bool markerBefore = false;
    bool markerAfter = false;

    bool IsNormal() const { return special == N; }
    bool IsSlipped() const { return special == P; }
    bool IsBoundOff() const { return special == BINDOFF; }
    bool IsBroken() const { return special == BREAK; }

    bool IsMade() const { return special == N || special == BREAK; }
    bool IsCountable() const { return special == N || special == P; }
};

static std::vector<Stitch> STITCHES {
    // BLUE vertical stitches are below
#define BLANK_STITCH_INDEX 0
        {
            "-", "slip/pick up/ignore", Stitch::P,
            1, 1,
        },
            // TODO support breaking (for vertical holes, split back, etc)
            // however, this requires some shenanigans in the layouting code
            // as separate sections don't have a stitch between them to pull them,
            // so it requires those sections to be grouped when resolving forces...
#define BREAK_STITCH_INDEX 1
        {
            "/", "break; unconnected to previous stitch on same row", Stitch::BREAK,
            0, 0,
        },
        {
            "pu", "pick up / slip", Stitch::N,
            1, 1,
            CIRCLE, RED,
            {
                { 0, 0, RED },
            }
        },
        {
            "co", "cast on", Stitch::N,
            0, 1,
            DOT, GREEN,
            {}
        },
        {
            "bo", "bind off", Stitch::BINDOFF,
            1, 1,
            VBAR, RED,
            {
                { 0, 0, BLUE }
            }
        },
        {
            "sk", "skip", Stitch::BINDOFF,
            1, 0,
            VBAR, BLUE,
        },
        {
            "k", "knit", Stitch::N,
            1, 1,
            DOT, BLACK,
            { 
                { 0, 0, BLACK },
            }
        },
        {
            "p", "purl", Stitch::N,
            1, 1,
            SQUIGGLE, BLUE,
            {
                { 0, 0, BLACK },
            }
        },
        {
            "k2tog", "knit two together", Stitch::N,
            2, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLUE },
                { 1, 0, BLACK },
            }
        },
        {
            "SSK", "slip slip knit", Stitch::N,
            2, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLACK },
                { 1, 0, BLUE },
            }
        },
        {
            "p2tog", "purl two together", Stitch::N,
            2, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLACK },
                { 1, 0, BLUE },
            }
        },
        {
            "SSP", "slip slip purl", Stitch::N,
            2, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLUE },
                { 1, 0, BLACK },
            }
        },
        {
            "Tss", "tunisian simple stitch", Stitch::N,
            1, 1,
            DOT, BLACK,
            {
                { 0, 0, BLACK },
            }
        },
        {
            "kfb", "knit front and back", Stitch::N,
            1, 2,
            ARROWDOWN, GREEN,
            {
                { 0, 0, GREEN },
                { 0, 1, BLACK },
            }
        },
        {
            "M", "make one", Stitch::N,
            0, 1,
            ARROWDOWN, GREEN,
            {
                { MapEntry::INBETWEEN , 0, GREEN },
            }
        },
        {
            "|", "Tunisian pick up", Stitch::N,
            1, 1,
            DOT, RED,
            {
                { 0, 0, RED },
            }
        },
        {
            "||", "Tunisian end of row", Stitch::N,
            1, 1,
            DOT, RED,
            {
                { 0, 0, GREEN },
            }
        },
        {
            "Xss", "crossed tunisian simple stitch", Stitch::N,
            2, 2,
            DOT, BLUE,
            {
                { 0, 1, BLACK },
                { 1, 0, BLUE },
            }
        },
        /*
        {
            "Tfs", "Tunisian full stitch", Stitch::N,
            1, 1,
            DOT, BLACK,
            {
                { MapEntry::INBETWEEN, 0, BLACK, CIRCLE },
            }
        },
        */
        {
            "Tfs", "Tunisian full stitch", Stitch::N,
            1, 1,
            DOT, BLACK,
            {
                { 0, 0, BLACK, CIRCLE },
            }
        },
        {
            "yo", "yarn over", Stitch::N,
            0, 1,
            CIRCLE, GREEN,
        },
        {
            "ml", "make 1 left", Stitch::N,
            0, 1,
            ARROWLEFT, GREEN,
            {
                { MapEntry::INBETWEEN, 0, BLACK }
            }
        },
        {
            "ma", "make 1 air", Stitch::N,
            0, 1,
            ARROWDOWN, GREEN,
            {
                { 0, 0, GREEN }
            }
        },
        {
            "mr", "make 1 right", Stitch::N,
            0, 1,
            ARROWRIGHT, GREEN,
            {
                { MapEntry::INBETWEEN, 0, BLUE }
            }
        },
        {
            "CDD", "central double decrease", Stitch::N,
            3, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLUE },
                { 1, 0, BLACK },
                { 2, 0, BLUE },
            }
        },
        {
            "SK2P", "sl k2tog psso", Stitch::N,
            3, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLACK },
                { 1, 0, BLUE },
                { 2, 0, BLUE },
            }
        },
        {
            "s", "generic stitch", Stitch::N,
            1, 1,
            DOT, BLACK,
            {
                { 0, 0, BLACK },
            }
        },
        {
            "2s", "generic stitch twice in same loop", Stitch::N,
            1, 2,
            ARROWDOWN, GREEN,
            {
                { 0, 0, BLACK },
                { 0, 1, GREEN }
            }
        },
        {
            "3s", "generic stitch thrice in same loop", Stitch::N,
            1, 3,
            ARROWDOWN, GREEN,
            {
                { 0, 0, BLACK },
                { 0, 1, GREEN },
                { 0, 2, GREEN }
            }
        },
        {
            "4s", "generic stitch 4 in same loop", Stitch::N,
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
            "5s", "generic stitch 5 in same loop", Stitch::N,
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
            "6s", "generic stitch 6 in same loop", Stitch::N,
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
            "7s", "generic stitch 7 in same loop", Stitch::N,
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
            "8s", "generic stitch 8 in same loop", Stitch::N,
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
            "s2tog", "generic stitch two together", Stitch::N,
            2, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLUE },
                { 1, 0, BLACK }
            }
        },
        {
            "s3tog", "generic stitch three together", Stitch::N,
            3, 1,
            ARROWUP, RED,
            {
                { 0, 0, BLUE },
                { 1, 0, BLUE },
                { 2, 0, BLACK },
            }
        },
        {
            "ch", "chain", Stitch::N,
            0, 1,
            CIRCLE, GREEN,
        },
        {
            "dc", "double crochet", Stitch::N,
            1, 1,
            DOT, BLACK,
            {
                { 0, 0, BLACK, HBAR },
            }
        },
        {
            "tc", "triple crochet", Stitch::N,
            1, 1,
            DOT, BLACK,
            {
                { 0, 0, BLACK, H2BAR },
            }
        },
        {
            "qc", "quadruple crochet", Stitch::N,
            1, 1,
            DOT, BLACK,
            {
                { 0, 0, BLACK, H3BAR },
            }
        },
        {
            "bb", "bobble", Stitch::N,
            1, 1,
            DOT, RED,
            {
                { 0, 0, RED, BOBBLE },
            }
        },
        {
            "c2f", "cable 2 front", Stitch::N,
            3, 3,
            DOT, BLACK,
            {
                { 0, 1, BLACK },
                { 1, 2, BLACK },
                { 2, 0, BLUE },
            }
        },
        {
            "c2b", "cable 2 back", Stitch::N,
            3, 3,
            DOT, BLUE,
            {
                { 0, 2, BLUE },
                { 1, 0, BLACK },
                { 2, 1, BLACK },
            }
        },
        {
            "c3b", "cable 3 back", Stitch::N,
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
            "c3f", "cable 3 front", Stitch::N,
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
            "c3b", "cable 3 back", Stitch::N,
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
