#ifndef DRAW_H
#define DRAW_H

#include "common.h"

extern void* initCanvas(int w, int h);
extern void drawLine(void*, Color color, int x0, int y0, int x1, int y1);
extern void drawMarker(void*, Marker marker, Color color, int x, int y);
extern void drawGlyph(void*, char c, Color color, int left, int top);
extern void writeCanvas(void*, const char* fname);
extern void destroyCanvas(void*);

#endif
