#include "draw.h"
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <functional>
#include <vector>
#include <png.h>

#define LOG(fmt, ...) fprintf(stderr, "%s@%s:%d: " fmt "\n", __func__, __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__);

static char SUPPORTED[] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '(',
    ')',
};

static int FONT[][9][9] = {
    //    0  1  2  3  4  5  6  7  8
    { // 0
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 1, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0, 0, 1, 0, 0 },
        { 0, 1, 0, 0, 0, 1, 0, 1, 0 },
        { 0, 1, 0, 1, 0, 0, 0, 1, 0 },
        { 0, 0, 1, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
    }, { // 1
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 1, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 0 },
    }, { // 2
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 1, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 0 },
    }, { // 3
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    }, { // 4
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 0, 0, 1, 0, 0, 0 },
        { 0, 0, 1, 0, 0, 1, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0 },
    }, { // 5
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 1, 0, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 0, 0, 0 },
    }, { // 6
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 1, 0, 0, 0, 1, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 1, 1, 0, 0, 0, 1, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    }, { // 7
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
    }, { // 8
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    }, { // 9
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 1, 0, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 0, 0, 0 },
    }, { // (
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0 },
    }, { // )
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 0, 0 },
    }
};

static int MARKERS[][5][5] = {
    { // DOT
        { 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 0 },
        { 0, 1, 1, 1, 0 },
        { 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0 },
    },
    { // CIRCLE
        { 0, 1, 1, 1, 0 },
        { 1, 1, 0, 1, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 1, 0, 1, 1 },
        { 0, 1, 1, 1, 0 },
    },
    { // SQUIGGLE
        { 0, 0, 0, 0, 0 },
        { 1, 1, 1, 0, 0 },
        { 0, 1, 1, 1, 0 },
        { 0, 0, 1, 1, 1 },
        { 0, 0, 0, 0, 0 },
    },
    { // ARROWDOWN
        { 1, 1, 1, 1, 1 },
        { 0, 1, 1, 1, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
    },
    { // ARROWUP
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 1, 1, 1, 0 },
        { 1, 1, 1, 1, 1 },
    },
    { // EXCLAMATION
        { 0, 1, 1, 0, 0 },
        { 0, 1, 1, 0, 0 },
        { 0, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 1, 1, 0, 0 },
    },
    { // HBAR
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
    },
    { // H2BAR
        { 0, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 0 },
    },
    { // H3BAR
        { 1, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 1 },
    },
    { // VBAR
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
    },
    { // ARROWLEFT
        { 0, 0, 1, 0, 0 },
        { 0, 1, 1, 0, 0 },
        { 1, 1, 1, 0, 0 },
        { 0, 1, 1, 0, 0 },
        { 0, 0, 1, 0, 0 },
    },
    { // ARROWRIGHT
        { 0, 0, 1, 0, 0 },
        { 0, 0, 1, 1, 0 },
        { 0, 0, 1, 1, 1 },
        { 0, 0, 1, 1, 0 },
        { 0, 0, 1, 0, 0 },
    },
    { // BOBBLE
        { 0, 0, 1, 0, 0 },
        { 0, 1, 1, 1, 0 },
        { 1, 0, 1, 0, 1 },
        { 0, 1, 1, 1, 0 },
        { 0, 0, 1, 0, 0 },
    },
};

struct RGB {
    uint8_t r, g, b;

    RGB(Color c)
        : r((c >> 16) & 0xFF)
        , g((c >> 8 ) & 0xFF)
        , b((c >> 0 ) & 0xFF)
    {}
};

struct Canvas {
    int w, h;
    png_bytep *rows;
    png_structp png_ptr;
    png_infop info_ptr;
    FILE* fp;
    std::vector<std::function<void(void)>> glyphs;
    std::vector<std::function<void(void)>> lines;
    std::vector<std::function<void(void)>> markers;
};

void _drawLine(void* hcanvas, Color color, int x0, int y0, int x1, int y1)
{
    auto* canvas = (Canvas*)hcanvas;
    int dx = x1 - x0;
    int dy = y1 - y0;
    RGB rgb(color);
    if(std::abs(dx) >= std::abs(dy)) {
        // advance on x; xstep is 1 or -1
        if(x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
            dx = x1 - x0;
            dy = y1 - y0;
        }
        int step = (dx < 0) ? -1 : 1;
        for(int i = 0; i <= std::abs(dx); ++i) {
            float y = y0 + (float(x0 + step * i) - x0) / dx * dy;
            assert(y >= 0.f);
            int iy = int(std::floor(y));
            float fy = y - std::floor(y);
            RGB rgb(color);
            canvas->rows[iy][(x0 + step * i) * 4 + 0] = rgb.r * (1.f - fy) + canvas->rows[iy][(x0 + step * i) * 4 + 0] * fy;
            canvas->rows[iy][(x0 + step * i) * 4 + 1] = rgb.g * (1.f - fy) + canvas->rows[iy][(x0 + step * i) * 4 + 1] * fy;
            canvas->rows[iy][(x0 + step * i) * 4 + 2] = rgb.b * (1.f - fy) + canvas->rows[iy][(x0 + step * i) * 4 + 2] * fy;
            canvas->rows[iy][(x0 + step * i) * 4 + 3] = 0xFF;
            canvas->rows[iy+1][(x0 + step * i) * 4 + 0] = rgb.r * (fy) + canvas->rows[iy+1][(x0 + step * i) * 4 + 0] * (1.f - fy);
            canvas->rows[iy+1][(x0 + step * i) * 4 + 1] = rgb.g * (fy) + canvas->rows[iy+1][(x0 + step * i) * 4 + 1] * (1.f - fy);
            canvas->rows[iy+1][(x0 + step * i) * 4 + 2] = rgb.b * (fy) + canvas->rows[iy+1][(x0 + step * i) * 4 + 2] * (1.f - fy);
            canvas->rows[iy+1][(x0 + step * i) * 4 + 3] = 0xFF;
        }
    } else {
        if(y0 > y1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
            dx = x1 - x0;
            dy = y1 - y0;
        }
        int step = (dy < 0) ? -1 : 1;
        for(int i = 0; i <= std::abs(dy); ++i) {
            float x = x0 + (float(y0 + step * i) - y0) / dy * dx;
            assert(x >= 0.f);
            int ix = int(std::floor(x));
            float fx = x - std::floor(x);
            // TODO refactor this out
            RGB rgb(color);
            canvas->rows[y0 + step * i][ix * 4 + 0] = rgb.r * (1.f - fx) + canvas->rows[y0 + step * i][ix * 4 + 0] * fx;
            canvas->rows[y0 + step * i][ix * 4 + 1] = rgb.g * (1.f - fx) + canvas->rows[y0 + step * i][ix * 4 + 1] * fx;
            canvas->rows[y0 + step * i][ix * 4 + 2] = rgb.b * (1.f - fx) + canvas->rows[y0 + step * i][ix * 4 + 2] * fx;
            canvas->rows[y0 + step * i][ix * 4 + 3] = 0xFF;
            canvas->rows[y0 + step * i][(ix+1) * 4 + 0] = rgb.r * (fx) + canvas->rows[y0 + step * i][(ix+1) * 4 + 0] * (1.f - fx);
            canvas->rows[y0 + step * i][(ix+1) * 4 + 1] = rgb.g * (fx) + canvas->rows[y0 + step * i][(ix+1) * 4 + 1] * (1.f - fx);
            canvas->rows[y0 + step * i][(ix+1) * 4 + 2] = rgb.b * (fx) + canvas->rows[y0 + step * i][(ix+1) * 4 + 2] * (1.f - fx);
            canvas->rows[y0 + step * i][(ix+1) * 4 + 3] = 0xFF;
        }
    }
}

void _drawMarker(void* hcanvas, Marker marker, Color color, int x, int y)
{
    auto* canvas = (Canvas*)hcanvas;
    RGB rgb(color);
    for(int i = 0; i < 5; ++i) {
        for(int j = 0; j < 5; ++j) {
            if(MARKERS[marker][i][j]) {
                canvas->rows[i - 2 + y][(j - 2 + x) * 4 + 0] = rgb.r;
                canvas->rows[i - 2 + y][(j - 2 + x) * 4 + 1] = rgb.g;
                canvas->rows[i - 2 + y][(j - 2 + x) * 4 + 2] = rgb.b;
                canvas->rows[i - 2 + y][(j - 2 + x) * 4 + 3] = 0xFF;
            }
        }
    }
}

void _drawGlyph(void* hcanvas, char c, Color color, int left, int top)
{
    int glyphIndex = 0;
    for(; glyphIndex < sizeof(SUPPORTED)/sizeof(SUPPORTED[0]); ++glyphIndex) {
        if(c == SUPPORTED[glyphIndex]) break;
    }
    if(glyphIndex >= sizeof(SUPPORTED)/sizeof(SUPPORTED[0])) return;

    auto* canvas = (Canvas*)hcanvas;

    RGB rgb(color);
    for(int y = 0; y < 9; ++y) {
        for(int x = 0; x < 9; ++x) {
            if(FONT[glyphIndex][y][x]) {
                canvas->rows[top + y][(left + x) * 4 + 0] = rgb.r;
                canvas->rows[top + y][(left + x) * 4 + 1] = rgb.g;
                canvas->rows[top + y][(left + x) * 4 + 2] = rgb.b;
                canvas->rows[top + y][(left + x) * 4 + 3] = 0xFF;
            }
        }
    }
}

void drawLine(void* hcanvas, Color color, int x0, int y0, int x1, int y1)
{
    static int counter = 0;
    LOG("drawLine: %d %p %06X %d,%d %d,%d", ++counter, hcanvas, color, x0, y0, x1, y1);
    auto* canvas = (Canvas*)hcanvas;
    assert(x0 >= 0 && x0 < canvas->w);
    assert(y0 >= 0 && y0 < canvas->h);
    assert(x1 >= 0 && x1 < canvas->w);
    assert(y1 >= 0 && y1 < canvas->h);
    int mycounter = counter;
    canvas->lines.emplace_back([=](){ LOG("drawLine: %d executing", mycounter); _drawLine(hcanvas, color, x0, y0, x1, y1); });
}

void drawGlyph(void* hcanvas, char c, Color color, int x, int y)
{
    LOG("drawGlyph: %p %c %06X %d,%d", hcanvas, c, color, x, y);
    auto* canvas = (Canvas*)hcanvas;
    assert(x >= 0 && x < canvas->w);
    assert(y >= 0 && y < canvas->h);
    canvas->glyphs.emplace_back([=](){ _drawGlyph(hcanvas, c, color, x, y); });
}

void drawMarker(void* hcanvas, Marker marker, Color color, int x, int y)
{
    LOG("drawMarker: %p %d %06X %d,%d", hcanvas, marker, color, x, y);
    if(marker == NONE) return;
    auto* canvas = (Canvas*)hcanvas;
    assert(x >= 0 && x < canvas->w);
    assert(y >= 0 && y < canvas->h);
    canvas->markers.emplace_back([=](){ _drawMarker(hcanvas, marker, color, x, y); });
}

void* initCanvas(int w, int h)
{
    Canvas* canvas = new Canvas;
    canvas->w = w;
    canvas->h = h;
    LOG("initCanvas: %p %d x %d", canvas, w, h);
    canvas->fp = NULL;
    canvas->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    canvas->info_ptr = png_create_info_struct(canvas->png_ptr);
    png_set_IHDR(canvas->png_ptr, canvas->info_ptr, canvas->w, canvas->h,
            8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    canvas->rows = (png_bytep*)malloc(sizeof(png_bytep) * h);
    for(int i = 0; i < canvas->h; ++i) {
        canvas->rows[i] = (png_bytep)malloc(png_get_rowbytes(canvas->png_ptr, canvas->info_ptr));
        std::memset(canvas->rows[i], 0xFF, png_get_rowbytes(canvas->png_ptr, canvas->info_ptr));
    }
    png_set_rows(canvas->png_ptr, canvas->info_ptr, canvas->rows);
    return canvas;
}

void destroyCanvas(void* hcanvas)
{
    LOG("destroyCanvas: %p", hcanvas);
    auto* canvas = (Canvas*)hcanvas;
    png_destroy_write_struct(&canvas->png_ptr, &canvas->info_ptr);
    for(int i = 0; i < canvas->h; ++i) {
        free(canvas->rows[i]);
    }
    free(canvas->rows);
    if(canvas->fp) fclose(canvas->fp);
    delete canvas;
}

void writeCanvas(void* hcanvas, const char* fname)
{
    LOG("writeCanvas: %p %s", hcanvas, fname);
    auto* canvas = (Canvas*)hcanvas;

    for(auto&& glyph : canvas->glyphs) glyph();
    for(auto&& line : canvas->lines) line();
    for(auto&& marker : canvas->markers) marker();

    //if(setjmp(png_jmpbuf(png_ptr))); TODO
    if(canvas->fp) fclose(canvas->fp); // TODO
    canvas->fp = fopen(fname, "wb");
    png_init_io(canvas->png_ptr, canvas->fp);
    png_write_png(canvas->png_ptr, canvas->info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
}
