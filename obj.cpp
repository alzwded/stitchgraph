#include "stitches.h"

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
using namespace std::literals;

#ifndef PI
# define PI 3.14159f
#endif
#ifndef PI2
# define PI2 (2.f * PI)
#endif

#define LOG(fmt, ...) fprintf(stderr, "%s@%s:%d: " fmt "\n", __func__, __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__);

struct Vertex {
    float x,y,z;
    float u = 0, v = 0;
};

struct Quad {
    ssize_t v[4] = { -1, -1, -1, -1 };
};

void WriteCylinder(std::string const& baseName, std::vector<std::vector<Dot>>& dots, int minx, int miny, int maxx, int maxy, int offx, int canvasWidth, float maxAngle)
{
    std::string objName = baseName + ".obj"s;
    std::string texName = baseName + ".png"s;
    std::string mtlName = baseName + ".mtl"s;
    std::string cleanBaseName = baseName;
    if(auto pos = cleanBaseName.rfind("/");
            pos != std::string::npos)
    {
        cleanBaseName = cleanBaseName.substr(pos+1);
    }
    if(auto pos = cleanBaseName.rfind("\\");
            pos != std::string::npos)
    {
        cleanBaseName = cleanBaseName.substr(pos+1);
    }
    std::vector<Vertex> verts;
    std::vector<Quad> quads;

    // determine max facets
    auto maxFacets = std::accumulate(dots.begin(), dots.end(), size_t(0), [](size_t acc, std::vector<Dot>& rowdots) -> size_t {
                return std::max(acc, std::accumulate(rowdots.begin(), rowdots.end(), size_t(0), [](size_t acc, Dot& d) -> size_t {
                            if(d.skip) return acc;
                            return acc + size_t(1);
                            }));
            });

    verts.reserve(maxFacets * dots.size());
    quads.reserve(verts.size() / 8);

    float y = 0.f;
    float prevR = 0.f, prevL = 0.f;
    bool broken = true;
    for(int row = 0; row < dots.size(); ++row) {
        auto& rowdots = dots[row];
        LOG("================================");
        LOG("row %d has %zd dots", row, rowdots.size());
        if(rowdots.size() == 0) {
            y += 9.f;
            prevR = 0.f;
            prevL = 0.f;
            broken = true;
            continue;
        }
        auto f1 = rowdots[0].x;
        auto f2 = rowdots[rowdots.size()-1].x;
        if(f1 > f2) std::swap(f1, f2);
        float l = f2 - f1;
        float r = l / PI2;

#if 0
        if(prevR == 0 && r == 0) { LOG("Y0"); y += 0.f; }
        else if(std::abs(r - prevR) >= 9.5) { LOG("Y1"); y += 0.f; }
        else if(std::abs(r - prevR) <= 2) { LOG("Y2"); y += 9.f; }
        else { LOG("Y3"); y += 5.f; }
#else
        float oldY = y;
        if(prevL == 0 && l == 0) { LOG("Y0"); y += 9.f; }
        else if(std::abs(l - prevL) > 9 * PI2) { LOG("Y1"); y += 0.f; }
        else if(std::abs(l - prevL) < 18) { LOG("Y2"); y += 9.f; }
        else { LOG("Y3"); y += 9.f - 6.f * ((std::abs(l - prevL) / (9 * PI2))); }
#endif

        LOG("y=%f r=%f prevR=%f l=%f prevL=%f l-prevl=%f dy=%f", y, r, prevR, l, prevL, l-prevL, y-oldY);
        prevR = r;
        prevL = l;



        for(int i = 0; i < maxFacets; ++i)
        {
            verts.emplace_back();
            float theta = float(i)/float(maxFacets) * maxAngle; // TODO this technically isn't correct with how we calculate the radius and how we determine the Y increase... it only works for 360deg
            verts.back().x = r * cos(theta);
            verts.back().y = y;
            verts.back().z = r * sin(theta);
            float localX = f1 + float(i)/float(maxFacets-1) * l;
            LOG("l=%f localX=%f i=%d maxFacets=%zd minx=%d maxx=%d canvasWidth=%d offx=%d", l, localX, i, maxFacets, minx, maxx, canvasWidth, offx);
            verts.back().u = (localX) / canvasWidth;
            verts.back().v = (miny + (row) * 9.f) / (maxy);
            LOG("u=%f v=%f ux=%f vy=%f", verts.back().u, verts.back().v, localX, row * 9.f + miny);
        }

        if(row == 0) { broken = false; continue; }
        if(broken) {
            broken = false;
            continue;
        }
        for(int i = 1; i < maxFacets; ++i) {
            quads.emplace_back();
            quads.back().v[0] = verts.size() - 2 * maxFacets + i - 0;
            quads.back().v[1] = verts.size() - 2 * maxFacets + i - 1;
            quads.back().v[2] = verts.size() - 1 * maxFacets + i - 1;
            quads.back().v[3] = verts.size() - 1 * maxFacets + i - 0;
            LOG("Q %zd %zd %zd %zd",
                    quads.back().v[0],
                    quads.back().v[1],
                    quads.back().v[2],
                    quads.back().v[3]);
        }
    }


    FILE* f = fopen(mtlName.c_str(), "w");
    fprintf(f, "newmtl M1\n");
    fprintf(f, "Ka 0.6 0.6 0.6\nKd 0.8 0.8 0.8\nKs 1.0 1.0 1.0\nNs 10\nd 1.0\n");
    fprintf(f, "map_Kd %s.png\n", cleanBaseName.c_str());
    fclose(f);
    f = fopen(objName.c_str(), "w");
    fprintf(f, "mtllib %s.mtl\n", cleanBaseName.c_str());
    fprintf(f, "usemtl M1\n");
    for(auto& v : verts) {
        fprintf(f, "v %f %f %f\n", v.x, v.y, v.z);
        fprintf(f, "vt %f %f\n", v.u, v.v);
    }

    for(auto& q : quads) {
        fprintf(f, "f %zd/%zd %zd/%zd %zd/%zd %zd/%zd\n",
                q.v[0]+1, q.v[0]+1,
                q.v[1]+1, q.v[1]+1,
                q.v[2]+1, q.v[2]+1,
                q.v[3]+1, q.v[3]+1);
    }
    fclose(f);
}
