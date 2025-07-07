/*
Copyright 2025 Vlad Meșco

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef DRAW_H
#define DRAW_H

#include "common.h"

[[nodiscard("return value must be freed with destroyCanvas")]]
extern void* initCanvas(int w, int h);
// additively draw an antialiased 1px wide line from x0,y0 to x1,y1
extern void drawLine(void*, Color color, int x0, int y0, int x1, int y1);
// draw a 5x5 marker centered at x,y
extern void drawMarker(void*, Marker marker, Color color, int x, int y);
// draw a 9x9 font glyph at x,y going down-right
extern void drawGlyph(void*, char c, Color color, int left, int top);
// save canvas to disk
extern void writeCanvas(void*, const char* fname);
// free memory allocated by initCanvas()
extern void destroyCanvas(void*);

#endif
