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
            0, 0,
        },
        {
            "co", "cast on", false,
            0, 1,
            DOT, GREEN,
            {}
        },
        {
            "bo", "bind off", false,
            1, 0,
        },
        {
            "sk", "skip", false,
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
};

#endif
