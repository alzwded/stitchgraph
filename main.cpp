#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>

#include "stitches.h"
#include "draw.h"

#define LOG(fmt, ...) fprintf(stderr, "%s@%s:%d: " fmt "\n", __func__, __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__);

using namespace std::literals;

struct Row
{
    typedef Stitch VALUE_TYPE;
    struct Iterator {
        Row& row;
        int incr;
        int at;
        Iterator(Row& _row, int _incr, int _at)
            : row(_row)
            , incr(_incr)
            , at(_at)
        {
            Normalize();
        }
        Iterator(Row& _row, bool _reversed)
            : row(_row)
            , incr(_reversed ? -1 : 1)
            , at(_reversed ? _row.stitches.size() - 1 : 0)
            {}
        Iterator(Iterator const& other)
            : row(other.row)
            , incr(other.incr)
            , at(other.at)
        {}
        Iterator& operator++()
        {
            at += incr;
            Normalize();
            return *this;
        }
        Iterator operator++(int)
        {
            at += incr;
            Normalize();
            return Iterator(row, incr, at - incr);
        }
        Iterator operator+(int arg)
        {
            return Iterator(row, incr, at + incr * arg);
        }
        Iterator operator-(int arg)
        {
            return Iterator(row, incr, at - incr * arg);
        }
        Iterator& operator+=(int arg)
        {
            at += incr * arg;
            Normalize();
            return *this;
        }
        Iterator& operator-=(int arg)
        {
            at -= incr * arg;
            Normalize();
            return *this;
        }
        operator bool() const
        {
            if(incr > 0) return at < row.stitches.size();
            else return at >= 0;
        }
        VALUE_TYPE& operator*()
        {
            return row.stitches[at];
        }
        VALUE_TYPE const& operator*() const
        {
            return row.stitches.at(at);
        }
        VALUE_TYPE* operator->()
        {
            return &row.stitches[at];
        }
        VALUE_TYPE const* operator->() const
        {
            return &row.stitches.at(at);
        }
        void Normalize()
        {
            if(incr > 0) {
                if(at >= row.stitches.size()) at = row.stitches.size();
            } else {
                if(at < -1) at = -1;
            }
        }
        void First()
        {
            if(incr) at = 0;
            else at = row.stitches.size() - 1;
        }
        void Last()
        {
            if(incr) at = row.stitches.size() - 1;
            else at = 0;
        }

        int operator-(Iterator const& other) const
        {
            return std::abs(at - other.at);
        }
    };

    bool reversed = false;
    bool turn = false;
    std::vector<VALUE_TYPE> stitches = {};
    int number = -1;
    int srcLine = -1;

    Iterator First(bool reversed)
    {
        return Iterator(*this, reversed);
    }
    Iterator Last(bool reversed)
    {
        Iterator it(*this, reversed);
        it.Last();
        return Iterator(it);
    }

    size_t CountMade() {
        return std::accumulate(stitches.begin(), stitches.end(), size_t(0), [](size_t acc, decltype(stitches)::const_reference e) -> size_t {
                //LOG("%d %d %s", e.puts, e.takes, e.blank ? "blank" : "");
                return acc + e.puts * size_t(!e.blank);
                });
    }
    size_t CountTaken() {
        return std::accumulate(stitches.begin(), stitches.end(), size_t(0), [](size_t acc, decltype(stitches)::const_reference e) -> size_t {
                return acc + e.takes;
                });
    }
    size_t CountAll() {
        return std::accumulate(stitches.begin(), stitches.end(), size_t(0), [](size_t acc, decltype(stitches)::const_reference e) -> size_t {
                return acc + e.puts;
                });
    }
};

// Represents a dot/knot we make on the canvas
//
// These abstract away the breaks and blanks and whatnot, these end up being the drawing
// instructions
struct Dot
{
    struct LineTo {
        Dot* dotRef = nullptr;
        MapEntry* lineInfo = nullptr;
    };

    int x = 0, y = 0; // canvas coordinates
    bool skip = false;
    Stitch* stitchRef = nullptr; // which stitch are we referencing (for DOT, or if it's blank or whatnot)
    std::list<LineTo> lines; // where do we draw rows to (prev on row, stitches below)
    Dot* dotRef = nullptr; // this stitch was slipped up / was a short row, so we reference a dot faaar below
};

Row parse_row(std::string const& insline)
{
    LOG("parse_row %s", insline.c_str());
    Row rval;
    std::string sline = insline;

    // remove comment
    auto foundHash = sline.find('#');
    if(foundHash != std::string::npos)
        sline = sline.substr(0, foundHash);

    // skip label
    auto foundColon = sline.find(':');
    if(foundColon != std::string::npos)
    {
        auto maybeNumber = sline.substr(0, foundColon);
        sline = sline.substr(foundColon + 1);
        // is it a number override:
        try {
            int n = std::stol(maybeNumber);
            rval.number = n;
            LOG("...row number overridden to %d", n);
        } catch(...) {
            // it wasn't a number
        }
    }

    LOG("...after stripping label and comments: %s", sline.c_str());
    std::stringstream is(sline);

    // extract parenthesised subexpression
    std::string s;
    auto parse_paren = [&is, &rval](std::string const& matching) {
        LOG("parse_paren looking for %s", matching.c_str());
        std::string s;
        std::stringstream next;
        // expect matching | *
        while(std::getline(is, s, ' ')) {
            // expect <matching>
            if(s == matching) break;
            // expect *
            next << s << ' ';
        }
        do {
            // expect count
            if(!std::getline(is, s, ' ')) {
                abort();
            }
        } while(s == "");

        // parse int count
        int rep = std::stoi(s);
        // parse the * part
        LOG("...rem is %s", next.str().c_str());
        Row intermed = parse_row(next.str());
        LOG("...adding above %d times", rep);
        for(int n = 0; n < rep; ++n) {
            // insert many stitches
            rval.stitches.insert(rval.stitches.end(), intermed.stitches.begin(), intermed.stitches.end());
        }
    };
    // encountered !
    bool marker = false;
    while(std::getline(is, s, ' ')) {
        // empty space goes whooosh!
        if(s.empty()) continue;
        // expect *, (, [
        if(s == "*") {
            parse_paren("*");
            continue;
        } else if(s == "(") {
            parse_paren(")");
            continue;
        } else if(s == "[") {
            parse_paren("]");
            continue;
        // expect end of line instructions
        } else if(s == "turn") {
            LOG("...turn");
            rval.reversed = true;
            rval.turn = true;
            if(marker) {
                LOG("...adding marker after");
                rval.stitches.back().markerAfter = true;
                marker = false;
            }
            return rval;
        } else if(s == "join" || s == "return") {
            LOG("...%s", s.c_str());
            if(marker) {
                LOG("...adding marker after");
                rval.stitches.back().markerAfter = true;
                marker = false;
            }
            return rval;
        // exclamation in-between marker
        } else if(s.substr(0, 1) == "!") {
            LOG("...found marker");
            marker = true;
            if(!rval.stitches.empty()) {
                rval.stitches.back().markerAfter = true;
                LOG("...adding marker after");
            } else {
                LOG("...marker is at begning of line");
            }
            continue;
        }

        // expect one of the stitches
        bool ok = false;
        for(int i = 0; i < STITCHES.size(); ++i) {
            // valid stitch
            if(s == STITCHES[i].key) {
                LOG("Found valid stitch %s", s.c_str());
                int rep = 1;
                // check if we have a count
                {
                    bool ok2 = true;
                    auto wasAt = is.tellg();
                    do {
                        if(!std::getline(is, s, ' ')) {
                            ok2 = false;
                            break;
                        }
                    } while(s.empty());
                    if(ok2) {
                        try {
                            rep = std::stoi(s);
                            LOG("...parsed count = %d", rep);
                        } catch(...) {
                            ok2 = false;
                        }
                    }
                    if(!ok2) {
                        LOG("...Couldn't parse count, resetting stream");
                        // we don't, put it back
                        is.seekg(wasAt);
                    }
                }

                // insert our stitches
                LOG("...Putting stitch %s %d times", STITCHES[i].key, rep);
                for(int n = 0; n < rep; ++n) {
                    rval.stitches.push_back(STITCHES[i]);
                    if(marker) {
                        LOG("...putting marker before first stitch");
                        rval.stitches.back().markerBefore = true;
                        marker = false;
                    }
                }
                ok = true;
                break;
            }
        }
        if(!ok) {
            printf("ERROR: %s is not a stitch\n", s.c_str());
            abort();
        }
    }
    return rval;
}

void usage(const char* argv0)
{
    printf("Usage: %s file\n", argv0);
    exit(2);
}

int main(int argc, char* argv[])
{
    // TODO getopt for version and help

    if(argc < 2) usage(argv[0]);

    if(strcmp(argv[1], "-h") == 0) usage(argv[0]);

    std::string fname = argv[1];
    std::fstream fin(fname, std::ios::in);

    std::vector<std::string> slines;
    std::string sline;
    while(fin.good()) {
        std::getline(fin, sline);
        slines.push_back(sline);
    }

    LOG("Parsing input");
    std::vector<Row> rows;
    // reversed = true means go left to right, = false means right to left
    bool reversed = true;
    size_t longest = 0; // FIXME count this by reducing "puts"
    int rowNumber = 0;
    int sourceLineNumber = 1;
    size_t previousLineLength = 0;
    for(auto&& l : slines) {
        LOG("Parsing %s on line %d", l.c_str(), sourceLineNumber);
        Row ll = parse_row(l);
        // ignore empty rows
        if(ll.stitches.empty()) {
            LOG("Row on line %d was empty", sourceLineNumber);
            sourceLineNumber++;
            continue;
        }
        // if the parser marked a line as 'reversed', it means we encounted a
        // turn of direction; this block normalizes the reversals so that
        // line.reversed is correct (false means right-to-left)
        if(ll.number >= 0) rowNumber = ll.number;
        else ll.number = rowNumber;
        ll.srcLine = sourceLineNumber;
        LOG("This is row %d, on source line %d", ll.number, ll.srcLine);
        rowNumber++;
        if(ll.reversed) {
            ll.reversed = reversed;
            reversed = !reversed;
        } else {
            ll.reversed = reversed;
        }
        sourceLineNumber++;
        LOG("Row's direction is %s", ll.reversed ? "left-to-right" : "right-to-left");
        /* 
        // TODO determine if we still need to reverse rows ever again
        if(!ll.reversed) { // always left-to-right for later
            std::reverse(ll.stitches.begin(), ll.stitches.end());
            std::for_each(ll.stitches.begin(), ll.stitches.end(), [](Stitch& st) {
                    std::swap(st.markerBefore, st.markerAfter);
                    std::for_each(st.map.begin(), st.map.end(), [&st](MapEntry& me) {
                            me.src = st.takes - 1 - me.src;
                            me.dst = st.puts - 1 - me.dst;
                            });
                    });
        }
        */

        size_t thisLen = ll.CountMade();
        LOG("Row's true length: %zd puts %zd takes", thisLen, ll.CountTaken());

        rows.push_back(ll);
        previousLineLength = thisLen;
        if(thisLen > longest) longest = thisLen;
    }

    LOG("Read %zd rows of stitching instructions", rows.size());
    LOG("");
    if(rows.empty()) {
        LOG("Nothing to do, exiting!");
        exit(0);
    }
    LOG("Normalizing short rows (if any)");

    // add blanks where we were short
    reversed = true; // reset reversed
    previousLineLength = rows[0].CountAll();
    for(int i = 1; i < rows.size(); ++i) {
        if(rows[i].CountTaken() > previousLineLength)
        {
            LOG("Row \"%d\" on source line %d is too long! (takes %zd, previous row was %zd/%zd)",
                    rows[i].number,
                    rows[i].srcLine,
                    rows[i].CountTaken(),
                    rows[i-1].CountMade(),
                    previousLineLength);
        }
        int delta = -rows[i].CountTaken() + rows[i - 1].CountAll();
        if(delta) {
            LOG("Row %d is short, need to add %d stitches", rows[i].number, delta);
            bool atEnd = true;
            if(atEnd) {
                for(int j = 0; j < delta; ++j) {
                    rows[i].stitches.push_back(STITCHES[BLANK_STITCH_INDEX]);
                }
            } else {
                Row newL = rows[i];
                newL.stitches.clear();
                for(int j = 0; j < delta; ++j) {
                    newL.stitches.push_back(STITCHES[BLANK_STITCH_INDEX]);
                }
                newL.stitches.insert(newL.stitches.end(), rows[i].stitches.begin(), rows[i].stitches.end());
                std::swap(newL, rows[i]);
            }
            if(rows[i].turn && i < rows.size() - 1) {
                LOG("This line turns, so we need to add %d stitches on the next row as well", delta);
                bool nextReversed = !reversed;
                atEnd = false;
                if(atEnd) {
                    for(int j = 0; j < delta; ++j) {
                        rows[i+1].stitches.push_back(STITCHES[BLANK_STITCH_INDEX]);
                    }
                } else {
                    Row newL = rows[i+1];
                    newL.stitches.clear();
                    for(int j = 0; j < delta; ++j) {
                        newL.stitches.push_back(STITCHES[BLANK_STITCH_INDEX]);
                    }
                    newL.stitches.insert(newL.stitches.end(), rows[i+1].stitches.begin(), rows[i+1].stitches.end());
                    std::swap(newL, rows[i+1]);
                }
            }
        }
        previousLineLength = rows[i].CountAll();
    }

    LOG("Instructions (simplified):");
    LOG("   +       increase area");
    LOG("   v       decrease area");
    LOG("   -       blank area");
    LOG("   =       bind off area");
    LOG("   |       normal area");
    for(int i = 0; i < rows.size(); ++i)
    {
        auto& ll = rows[i];
        printf("%3d: %c   ", ll.number, ll.reversed ? '>' : ' ');
        auto it = ll.First(!ll.reversed);
        while(it) {
            if(it->takes > it->puts) {
                int i = 0;
                for(; i < it->puts; ++i) printf("v ");
                for(; i < it->takes; ++i) printf("  ");
            }
            else if(it->takes < it->puts) {
                int i = 0;
                for(; i < it->takes; ++i) printf("| ");
                for(; i < it->puts; ++i) printf("+ ");
            }
            else if(it->IsSlipped()) { for(int i = 0; i < it->takes; ++i) printf("- "); }
            else if(it->map.empty()) { for(int i = 0; i < it->takes; ++i) printf("= "); }
            else {
                for(int i = 0; i < it->puts; ++i) printf("| ");
            }
            ++it;
        }
        printf("\t%c\t\t(@%d)\n", ll.reversed ? ' ' : '<', ll.srcLine);
    }
    LOG("");
    LOG("Populating canvas");
    exit(42);


    // TODO new code above this line, purge remaining dead code
    auto hcanvas = initCanvas(3 * 9 + longest * 9 + 3 * 9 + 5 * 9, (rows.size() + 3) * 9);

    // first pass -- determine where dots sit, because we need to stitch neighbours together
    std::vector<std::vector<Dot>> dots;
    for(int i = 0; i < rows.size(); ++i) {
        dots.emplace_back();

        std::string n = std::to_string(i);
        // draw line number
        for(int z = 0; i > 0 && z < n.size(); ++z) {
            int x = 0;
            if(!rows[i].reversed) {
                x = n.size() - 1 - z;
                x += longest + 3;
            } else {
                x = 2 - z;
            }
            x *= 9;
            drawGlyph(hcanvas, n[n.size() - 1 - z], BLACK, x, (2 + rows.size() - 1 - i) * 9 - 3);
        }
        // draw loop counter
        n = std::string("(") + std::to_string(std::accumulate(rows[i].stitches.begin(), rows[i].stitches.end(), 0, [](int acc, Stitch const& st) -> int {
                        return acc + (!st.blank) * st.puts;
                        })) + ")";
        for(int z = 0; z < n.size(); ++z) {
            int x = 3 + longest + 3;
            x *= 9;
            x += (4 - z) * 9;
            drawGlyph(hcanvas, n[n.size() - 1 - z], BLACK, x, (2 + rows.size() - 1- i) * 9 - 3);
        }

        int lineLen = std::accumulate(rows[i].stitches.begin(), rows[i].stitches.end(), 0, [](int acc, Stitch const& st) -> int {
                return acc + st.puts;
                });

        // generate dots
        int cc = 0;
        for(int j = 0; j < rows[i].stitches.size(); ++j) {
            // TODO
            // if this sits above a blank, it magnetizes downwards by
            // N-4 px, where N is the amount of blanks beneath it (short rows)
            // TODO magnetize up if part of dec, magnetize down if part
            //      of inc

            for(int k = 0; k < rows[i].stitches[j].puts; ++k) {
                dots.back().emplace_back();
                dots.back().back().skip = rows[i].stitches[j].blank;
                dots.back().back().y = (1 + rows.size() - 1 - i) * 9 + 7;
                auto delta = (longest - lineLen) * 9;
                dots.back().back().x = delta / 2 + cc * 9 + 3 * 9 + 4;
                ++cc;
            } // for each put stitch
        } // for each stitch
        printf("Line %d used cc = %d out of lineLen = %d\n", i, cc, lineLen);
    } // for each line

    // second pass -- generate draw instructions
    for(int i = 0; i < rows.size(); ++i) {
        printf("==========\n");
        printf("line %d\n", i);
        printf("----------\n");
        int cc = 0;
        int pc = 0;
        //while(cc < dots[i].size() && dots[i][cc].skip) cc++;
        //if(i > 0) while(pc < dots[i-1].size() && dots[i-1][pc].skip) pc++;
        for(int j = 0; j < rows[i].stitches.size(); ++j) {
            if(rows[i].stitches[j].markerBefore) {
                printf("Marker before st %d\n", j);
                drawMarker(hcanvas, EXCLAMATION, RED, dots[i][cc].x - 4, dots[i][cc].y);
            }
            if(rows[i].stitches[j].markerAfter) {
                printf("Marker after st %d\n", j);
                drawMarker(hcanvas, EXCLAMATION, RED, dots[i][cc].x + 5 + 9 * (std::max(0, rows[i].stitches[j].puts - 1)), dots[i][cc].y);
            }
            for(int put = 0; put < rows[i].stitches[j].puts; ++put) {
                if(!rows[i].stitches[j].blank) {
                    if(cc > 0 && !dots[i][cc-1].skip) {
                        drawLine(hcanvas, BLACK, dots[i][cc-1].x, dots[i][cc-1].y, dots[i][cc].x, dots[i][cc].y);
                    }
                    if(cc < dots[i].size() - 1 && !dots[i][cc+1].skip) {
                        drawLine(hcanvas, BLACK, dots[i][cc+1].x, dots[i][cc+1].y, dots[i][cc].x, dots[i][cc].y);
                    }
                    drawMarker(hcanvas, rows[i].stitches[j].marker, rows[i].stitches[j].color, dots[i][cc].x, dots[i][cc].y);
                    if(rows[i].stitches[j].takes == 0 && !rows[i].stitches[j].map.empty()) {
                        drawLine(hcanvas, rows[i].stitches[j].map[0].color, dots[i-1][pc].x - 5, dots[i-1][pc].y, dots[i][cc].x, dots[i][cc].y);
                    }
                    for(int taken = 0; taken < rows[i].stitches[j].takes; ++taken) {
                        // find combination in map
                        for(int z = 0; z < rows[i].stitches[j].map.size(); ++z) {
                            if(rows[i].stitches[j].map[z].src == taken
                                    && rows[i].stitches[j].map[z].dst == put)
                            {
                                auto NTH_TAKEN = [&](int pc, int taken) {
                                    printf("NTH_TAKEN: pc = %d taken = %d ", pc, taken);
                                    int rval = pc;
                                    while(rval < dots[i-1].size() && dots[i-1][rval].skip) rval++;
                                    for(int j = 0; j < taken; ++j) {
                                        printf(" >> ?%d ?%zu :: ", rval, dots[i-1].size());
                                        if(rval >= dots[i-1].size()) break;
                                        while(rval < dots[i-1].size() && dots[i-1][rval].skip) rval++;
                                        rval++;
                                        printf(" ?%d ", rval);
                                    }
                                    printf("    rval = %d\n", rval);
                                    return rval;
                                };
                                auto pc_taken = NTH_TAKEN(pc, taken);
                                printf("    put = %d taken = %d src = %d dst = %d color = %06X\n",
                                        put, taken, rows[i].stitches[j].map[z].src,rows[i].stitches[j].map[z].dst,rows[i].stitches[j].map[z].color);
                                drawLine(hcanvas, rows[i].stitches[j].map[z].color, dots[i-1][pc_taken].x, dots[i-1][pc_taken].y, dots[i][cc].x, dots[i][cc].y);
                            }
                        } // for each MapEntry
                    } // for each taken
                }
                if(cc < dots[i].size()) {
                    cc++;
                    //while(cc < dots[i].size() && dots[i][cc].skip) ++cc;
                }
            } // for each put
            printf("pc pre = %d ", pc);
            for(int tt = 0; i > 0 && tt < rows[i].stitches[j].takes; ++tt) {
                if(pc >= dots[i-1].size()) break;
                if(!rows[i].stitches[j].blank) {
                    while(pc < dots[i-1].size() && dots[i-1][pc].skip) pc++;
                }
                ++pc;
            }
            printf("pc post = %d\n", pc);
        } // for each stitch
    } // for each line

    auto graphFname = fname + ".png"s;

    writeCanvas(hcanvas, graphFname.c_str());

    return 0;
}
