#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "stitches.h"
#include "draw.h"

struct Line
{
    bool reversed;
    std::vector<Stitch> stitches;
};

struct Dot
{
    int x, y;
    bool skip;
};

Line parse_line(std::string const& sline)
{
    Line rval = {
        false,
        {}
    };
    std::stringstream is(sline);
    std::string s;
    auto parse_paren = [&is, &rval](std::string const& matching) {
        std::string s;
        std::stringstream next;
        while(std::getline(is, s, ' ')) {
            if(s == matching) break;
            next << s << ' ';
        }
        if(!std::getline(is, s, ' ')) {
            abort();
        }
        int rep = std::stoi(s);
        Line intermed = parse_line(next.str());
        for(int n = 0; n < rep; ++n) {
            rval.stitches.insert(rval.stitches.end(), intermed.stitches.begin(), intermed.stitches.end());
        }
    };
    bool marker = false;
    while(std::getline(is, s, ' ')) {
        if(s == "*") {
            parse_paren("*");
            continue;
        } else if(s == "(") {
            parse_paren(")");
            continue;
        } else if(s == "[") {
            parse_paren("]");
            continue;
        } else if(s == "turn") {
            rval.reversed = true;
            if(marker) {
                rval.stitches.back().markerAfter = true;
                marker = false;
            }
            return rval;
        } else if(s.substr(0, 1) == "!") {
            marker = true;
            continue;
        }

        bool ok = false;
        for(int i = 0; i < STITCHES.size(); ++i) {
            if(s == STITCHES[i].key) {
                if(!std::getline(is, s, ' ')) {
                    abort();
                }
                int rep = std::stoi(s);

                for(int n = 0; n < rep; ++n) {
                    rval.stitches.push_back(STITCHES[i]);
                    if(marker) {
                        rval.stitches.back().markerBefore = true;
                        marker = false;
                    }
                }
                ok = true;
                break;
            }
        }
        if(!ok) {
            printf("WARN: %s is not a stitch\n", s.c_str());
        }
    }
    return rval;
}

int main(int argc, char* argv[]) {
    // DEBUG
#if 0
    auto* H = initCanvas(100, 100);
    drawLine(H, BLACK, 10, 10, 50, 90);
    drawLine(H, BLACK, 10, 10, 90, 50);
    writeCanvas(H, "__test.png");
    return 0;
#endif
    // DEBUG
    // TODO getopt for version and help

    std::vector<std::string> slines;
    std::string sline;
    while(std::cin.good()) {
        std::getline(std::cin, sline);
        slines.push_back(sline);
    }

    std::vector<Line> lines;
    bool reversed = true;
    size_t longest = 0; // FIXME count this by reducing "puts"
    for(auto&& l : slines) {
        Line ll = parse_line(l);
        if(ll.reversed) {
            ll.reversed = reversed;
            reversed = !reversed;
        } else {
            ll.reversed = reversed;
        }
        if(!ll.reversed) { // always left-to-right for later
            std::reverse(ll.stitches.begin(), ll.stitches.end());
            std::for_each(ll.stitches.begin(), ll.stitches.end(), [](Stitch& st) {
                    std::swap(st.markerBefore, st.markerAfter);
                    });
        }
        if(ll.stitches.size() > 0) {
            lines.push_back(ll);
            size_t thisLen = std::accumulate(ll.stitches.begin(), ll.stitches.end(), size_t(0), [](size_t acc, decltype(ll.stitches)::const_reference e) -> size_t {
                    return e.puts + e.blank + acc;
                    });
            if(thisLen > longest) longest = thisLen;
        }
    }

    auto hcanvas = initCanvas(3 * 9 + longest * 9 + 3 * 9 + 5 * 9, (lines.size() + 3) * 9);

    // first pass -- determine where dots sit, because we need to stitch neighbours together
    std::vector<std::vector<Dot>> dots;
    for(int i = 0; i < lines.size(); ++i) {
        dots.emplace_back();

        std::string n = std::to_string(i);
        // draw line number
        for(int z = 0; i > 0 && z < n.size(); ++z) {
            int x = 0;
            if(!lines[i].reversed) {
                x = n.size() - 1 - z;
                x += longest + 3;
            } else {
                x = 2 - z;
            }
            x *= 9;
            drawGlyph(hcanvas, n[n.size() - 1 - z], BLACK, x, (2 + lines.size() - 1 - i) * 9 - 3);
        }
        // draw loop counter
        n = std::string("(") + std::to_string(std::accumulate(lines[i].stitches.begin(), lines[i].stitches.end(), 0, [](int acc, Stitch const& st) -> int {
                        return acc + st.puts;
                        })) + ")";
        for(int z = 0; z < n.size(); ++z) {
            int x = 3 + longest + 3;
            x *= 9;
            x += (4 - z) * 9;
            drawGlyph(hcanvas, n[n.size() - 1 - z], BLACK, x, (2 + lines.size() - 1- i) * 9 - 3);
        }

        int lineLen = std::accumulate(lines[i].stitches.begin(), lines[i].stitches.end(), 0, [](int acc, Stitch const& st) -> int {
                return acc + st.puts + st.blank + (st.puts == 0 ? st.takes : 0);
                });

        int cc = 0;
        for(int j = 0; j < lines[i].stitches.size(); ++j) {
            // TODO
            // if this sits above a blank, it magnetizes downwards by
            // N-4 px, where N is the amount of blanks beneath it (short rows)
            // TODO magnetize up if part of dec, magnetize down if part
            //      of inc

            if(lines[i].stitches[j].blank) {
                dots.back().emplace_back();
                dots.back().back().x = 0;
                dots.back().back().y = 0;
                dots.back().back().skip = true;
                ++cc;
            }

            if(lines[i].stitches[j].puts == 0) { // if bind off
                for(int k = 0; k < lines[i].stitches[j].takes; ++k) {
                    dots.back().emplace_back();
                    dots.back().back().skip = true;
                    dots.back().back().y = (1 + lines.size() - 1 - i) * 9 + 7;
                    auto delta = (longest - lineLen) * 9;
                    dots.back().back().x = delta / 2 + cc * 9 + 3 * 9 + 4;
                    ++cc;
                }
            } else { // if !bind off
                for(int k = 0; k < lines[i].stitches[j].puts; ++k) {
                    dots.back().emplace_back();
                    dots.back().back().skip = false;
                    dots.back().back().y = (1 + lines.size() - 1 - i) * 9 + 7;
                    auto delta = (longest - lineLen) * 9;
                    dots.back().back().x = delta / 2 + cc * 9 + 3 * 9 + 4;
                    ++cc;
                } // for each put stitch
            } // if !bind off
        } // for each stitch
        printf("Line %d used cc = %d out of lineLen = %d\n", i, cc, lineLen);
    } // for each line

    // second pass -- generate draw instructions
    for(int i = 0; i < lines.size(); ++i) {
        printf("==========\n");
        printf("line %d\n", i);
        printf("----------\n");
        int cc = 0;
        int pc = 0;
        while(cc < dots[i].size() && dots[i][cc].skip) cc++;
        if(i > 0) while(pc < dots[i-1].size() && dots[i-1][pc].skip) pc++;
        for(int j = 0; j < lines[i].stitches.size(); ++j) {
            if(lines[i].stitches[j].markerBefore) {
                drawMarker(hcanvas, EXCLAMATION, RED, dots[i][cc].x - 4, dots[i][cc].y);
            }
            if(lines[i].stitches[j].markerAfter) {
                drawMarker(hcanvas, EXCLAMATION, RED, dots[i][cc].x + 5, dots[i][cc].y);
            }
            if(lines[i].stitches[j].blank) continue;
            for(int put = 0; put < lines[i].stitches[j].puts; ++put) {
                if(cc > 0 && !dots[i][cc-1].skip) {
                    drawLine(hcanvas, BLACK, dots[i][cc-1].x, dots[i][cc-1].y, dots[i][cc].x, dots[i][cc].y);
                }
                if(cc < dots[i].size() - 1 && !dots[i][cc+1].skip) {
                    drawLine(hcanvas, BLACK, dots[i][cc+1].x, dots[i][cc+1].y, dots[i][cc].x, dots[i][cc].y);
                }
                drawMarker(hcanvas, lines[i].stitches[j].marker, lines[i].stitches[j].color, dots[i][cc].x, dots[i][cc].y);
                for(int taken = 0; taken < lines[i].stitches[j].takes; ++taken) {
                    // find combination in map
                    for(int z = 0; z < lines[i].stitches[j].map.size(); ++z) {
                        if(lines[i].stitches[j].map[z].src == taken
                                && lines[i].stitches[j].map[z].dst == put)
                        {
                            auto NTH_TAKEN = [&](int pc, int taken) {
                                printf("NTH_TAKEN: pc = %d taken = %d ", pc, taken);
                                int rval = pc;
                                for(int j = 0; j < taken; ++j) {
                                    printf(" >> ?%d ?%zu :: ", rval, dots[i-1].size());
                                    if(rval >= dots[i-1].size()) break;
                                    rval++;
                                    printf(" ?%d ", rval);
                                    while(rval < dots[i-1].size() && dots[i-1][rval].skip) rval++;
                                }
                                printf("    rval = %d\n", rval);
                                return rval;
                            };
                            auto pc_taken = NTH_TAKEN(pc, taken);
                            drawLine(hcanvas, lines[i].stitches[j].map[z].color, dots[i-1][pc_taken].x, dots[i-1][pc_taken].y, dots[i][cc].x, dots[i][cc].y);
                        }
                    } // for each MapEntry
                } // for each taken
                if(cc < dots[i].size()) {
                    cc++;
                    while(cc < dots[i].size() && dots[i][cc].skip) ++cc;
                }
            } // for each put
            printf("pc pre = %d ", pc);
            for(int tt = 0; tt < lines[i].stitches[j].takes; ++tt) {
                if(pc >= dots[i-1].size()) break;
                ++pc;
                while(pc < dots[i-1].size() && dots[i-1][pc].skip) pc++;
            }
            printf("pc post = %d\n", pc);
        } // for each stitch
    } // for each line

    writeCanvas(hcanvas, "graph.png");

    return 0;
}
