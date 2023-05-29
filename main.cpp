#include <cstdio>
#include <cstdlib>
#include <cstring>
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
            return rval;
        }

        for(int i = 0; i < STITCHES.size(); ++i) {
            if(s == STITCHES[i].key) {
                if(!std::getline(is, s, ' ')) {
                    abort();
                }
                int rep = std::stoi(s);

                for(int n = 0; n < rep; ++n) {
                    rval.stitches.push_back(STITCHES[i]);
                }
                break;
            }
        }
    }
    return rval;
}

int main(int argc, char* argv[]) {
    // TODO getopt for version and help

    std::vector<std::string> slines;
    std::string sline;
    while(std::cin.good()) {
        std::getline(std::cin, sline);
        slines.push_back(sline);
    }

    std::vector<Line> lines;
    bool reversed = false;
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
        }
        lines.push_back(ll);
        if(ll.stitches.size() > longest) longest = ll.stitches.size();
    }

    // first pass -- determine where dots sit, because we need to stitch neighbours together
    std::vector<std::vector<std::pair<int, int>>> dots;
    for(int i = 0; i < lines.size(); ++i) {
        //size_t lineLen = lines[i].stitches.size(); // FIXME reduce line::stitches::puts
        dots.emplace_back();
        //dots.back().resize(lineLen);

        std::string n = std::tostring(i);
        for(int z = 0; i > 0 && z < n.size(); ++z) {
            int x = 2 - z;
            if(lines[i].reversed) {
                x += longest;
            }
            x *= 9;
            drawGlyph(n[n.size() - 1 - z], x, (2 + lines.size() - i) * 9 - 4);
        }
        n = std::string("(") + std::tostring(lines[i].stitches.size()) + ")";
        for(int z = 0; i > 0 && z < n.size(); ++z) {
            x = 3 + longest + 3;
            x *= 9;
            drawGlyph(n[n.size() - 1 - z], BLACK, x, (2 + lines.size() - i) * 9 - 4);
        }

        for(int j = 0; j < lines[i].stitches.size(); ++j) {
            // TODO
            // if this sits above a blank, it magnetizes downwards by
            // N-4 px, where N is the amount of blanks beneath it (short rows)
            // TODO magnetize up if part of dec, magnetize down if part
            //      of inc

            for(int k = 0; k < lines[i].stitches[j].puts; ++k) {
                dots.back().emplace_back();
                dots.back().back().second = (2 + lines.size() - i) * 9 + 8;
                auto delta = (longest - lines[i].stitches.size()) * 9;
                dots.back().back().first = delta / 2 + j * 9;
            } // for each put stitch
        } // for each stitch
    } // for each lien

    // second pass -- generate draw instructions
    for(int i = 0; i < lines.size(); ++i) {
        int cc = 0;
        int pc = 0;
        for(int j = 0; j < lines[i].stitches.size(); ++j) {
            for(int put = 0; put < lines[i].stitches[j].puts; ++put) {
                if(cc > 0) {
                    drawLine(BLACK, dots[i][j-1].first, dots[i][j-1].second, dots[i][cc].first, dots[i][cc].second);
                }
                if(cc < dots[i].size() - 1) {
                    drawLine(BLACK, dots[i][cc+1].first, dots[i][cc+1].second, dots[i][cc].first, dots[i][cc].second);
                }
                drawMarker(lines[i].stitches[j].marker, lines[i].stitches[j].color, dots[i][cc].first, dots[i][cc].second);
                for(int taken = 0; taken < lines[i].stitches[j].takes; ++taken) {
                    // find combination in map
                    for(int z = 0; z < lines[i].stitches[j].map.size(); ++z) {
                        if(lines[i].stitches[j].map[z].src == taken
                                && lines[i].stitches[j].map[z].dest == put)
                        {
                            drawLine(lines[i].stitches[j].map[z].color, dots[i-1][pc+taken].first, dots[i-1][pc+taken].second, dots[i][cc].first, dots[i][cc].second);
                            break;
                        }
                    } // for each MapEntry
                } // for each taken
                cc++;
            } // for each put
            pc += lines[i].stitches[j].takes;
        } // for each stitch
    } // for each line

    return 0;
}
