#ifndef DRAW_H
#define DRAW_H

#include "common.h"

extern void drawLine(Color color, int x0, int y0, int x1, int y1);
extern void drawMarker(Marker marker, Color color, int x, int y);
extern void drawGlyph(char c, Color color, int left, int top);

#endif
