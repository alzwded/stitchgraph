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
    const char* key;
    const char* description;
    bool blank;
    int takes;
    int puts;
    Marker marker;
    Color color;
    std::vector<MapEntry> map;
};

static std::vector<Stitch> STITCHES {
        {
            "-", "(blank)", true,
            0, 0,
        },
        {
            "co", "cast on", false,
            0, 1,
            DOT, RED,
            {}
        },
        {
            "bo", "bind off", false,
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
            SQUIGGLE, BLACK,
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
        },
        {
            "|", "Tunisian pick up", false,
            1, 1,
            DOT, RED,
        },
        {
            "||", "Tunisian end of row", false,
            1, 1,
            DOT, RED,
        },
        {
            "Xss", "crossed tunisian simple stitch", false,
            2, 2,
            DOT, BLUE,
            {
                { 1, 0, BLUE },
                { 0, 1, BLACK },
            }
        },
        {
            "yo", "yarn over", false,
            0, 1,
            CIRCLE, GREEN,
        },
};

#endif
