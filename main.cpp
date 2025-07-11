/*
Copyright 2025 Vlad Meșco

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
// stdlib
#include <cmath>
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

// platform
#include <unistd.h> // for getopt

// own
#include "stitches.h"
#include "draw.h"

#define LOG(fmt, ...) fprintf(stderr, "%s@%s:%d: " fmt "\n", __func__, __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__);

#define ACTUAL_TRAP_INSTRUCTION __asm("int3")
// usage: DEBUGGER(if(x == 4));
//   or:
//        DEBUGGER(auto potato = get_potato(); if(potato.Invariant()));
#define DEBUGGER(CODE) do{\
    CODE { \
        ACTUAL_TRAP_INSTRUCTION;\
    } \
}while(0)

using namespace std::literals;

// optimistic programming: nobody will change the definition from "black line without marker" :-)
static MapEntry connectingStitch;

struct Row
{
    typedef Stitch VALUE_TYPE;
    struct Iterator {
        Row& row;
        ssize_t incr;
        ssize_t at;
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

    // used for running count, i.e. (8) at end of line in rendering
    size_t CountNormal() {
        return std::accumulate(stitches.begin(), stitches.end(), size_t(0), [](size_t acc, decltype(stitches)::const_reference e) -> size_t {
                //LOG("%d %d %s", e.puts, e.takes, e.blank ? "blank" : "");
                return acc + e.puts * size_t(e.IsNormal());
                });
    }
    // Used for counting how many new stitches are put, for validating we haven't "taken" more than we had on the previous line
    size_t CountMade() {
        return std::accumulate(stitches.begin(), stitches.end(), size_t(0), [](size_t acc, decltype(stitches)::const_reference e) -> size_t {
                //LOG("%d %d %s", e.puts, e.takes, e.blank ? "blank" : "");
                return acc + e.puts * size_t(e.IsMade());
                });
    }
    // Used for sanity check and for layouting
    size_t CountTaken() {
        return std::accumulate(stitches.begin(), stitches.end(), size_t(0), [](size_t acc, decltype(stitches)::const_reference e) -> size_t {
                return acc + e.takes;
                });
    }
    // Used for sanity check and for layouting
    size_t CountStitchable() {
        return std::accumulate(stitches.begin(), stitches.end(), size_t(0), [](size_t acc, decltype(stitches)::const_reference e) -> size_t {
                return acc + e.puts * size_t(e.IsCountable());
                });
    }
    // Used to determine how much memory we need to allocate for the dot grid; dots which aren't rendered (e.g. break '/') still have to live somewhere
    size_t CountLiterallyAllPuts() {
        return std::accumulate(stitches.begin(), stitches.end(), size_t(0), [](size_t acc, decltype(stitches)::const_reference e) -> size_t {
                return acc + e.puts;
                });
    }
};

// Parse a row of stitches per stitches.h and returns a Row
//
// Reentrant -- calls itself recursively if it encounters *, ( or [
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
    // encountered '!' marker
    //
    // If we have a "previous stitch" of the "N" variety on a row, it will just attach itself to that stitch;
    // otherwise, it remains pending and it will attach itself to right before the next available renderable normal stitch
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
                marker = false;
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
                            size_t ncharparsed = -1;
                            int rep1 = std::stoi(s, &ncharparsed);
                            if(ncharparsed == s.size()) {
                                rep = rep1;
                                LOG("...parsed count = %d", rep);
                            } else {
                                LOG("...there were some extra characters in %s, probably a stitch and not a count", s.c_str());
                                ok2 = false;
                            }
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
                    rval.stitches.push_back(STITCHES[i]); // make a copy!
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

// Process one stitch file and produce one PNG file
void process_stitch_file(std::string const& fname)
{
    std::fstream fin(fname, std::ios::in);

    // read in stitch file
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
        ll.reversed = reversed;
        if(ll.turn) {
            reversed = !reversed;
        }
        sourceLineNumber++;
        LOG("Row's direction is %s", ll.reversed ? "left-to-right" : "right-to-left");

        size_t thisLen = ll.CountMade();
        LOG("Row's true length: %zd puts %zd takes", thisLen, ll.CountTaken());

        rows.push_back(ll);
        previousLineLength = thisLen;

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
    previousLineLength = rows[0].CountStitchable();
    if(rows[0].CountTaken() > 0) {
        LOG("Row 0 takes stitches, but you have none defined! Cast some on");
        abort();
    }
    for(int i = 1; i < rows.size(); ++i) {
        LOG("Prev %zd current %zd", previousLineLength, rows[i].CountTaken());
        if(rows[i].CountTaken() > previousLineLength)
        {
            LOG("Row \"%d\" on source line %d is too long! (takes %zd, previous row was %zd/%zd)",
                    rows[i].number,
                    rows[i].srcLine,
                    rows[i].CountTaken(),
                    rows[i-1].CountMade(),
                    previousLineLength);
            abort();
        }
        int delta = -rows[i].CountTaken() + rows[i - 1].CountStitchable();
        if(delta) {
            LOG("Row %d@%d is short, need to add %d stitches", rows[i].number, rows[i].srcLine, delta);
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
        previousLineLength = rows[i].CountStitchable();
    }

    // figure out how much memory we need to allocate for dots
    size_t longest = 0;
    for(int i = 1; i < rows.size(); ++i) {
        if(auto allPuts = rows[i].CountLiterallyAllPuts();
                allPuts > longest)
            longest = allPuts;
    }
    LOG("There are %zd rows of up to %zd stitches", rows.size(), longest);
    LOG("");

    LOG("Instructions (simplified):");
    LOG("   + &     increase area");
    LOG("   v .     decrease area");
    LOG("   -       slipped area");
    LOG("   =       bind off area");
    LOG("   |       normal area");
    for(int i = 0; i < rows.size(); ++i)
    {
        auto& ll = rows[i];
        fprintf(stderr, "%3d: %c   ", ll.number, ll.reversed ? '>' : ' ');
        auto it = ll.First(!ll.reversed);
        while(it) {
            if(it->takes > it->puts) {
                int i = 0;
                for(; i < it->puts; ++i)  fprintf(stderr, "v ");
                for(; i < it->takes; ++i) fprintf(stderr, ". ");
            }
            else if(it->takes < it->puts) {
                int i = 0;
                for(; i < it->takes; ++i) fprintf(stderr, "& ");
                for(; i < it->puts; ++i)  fprintf(stderr, "+ ");
            }
            else if(it->IsSlipped()) { for(int i = 0; i < it->takes; ++i) fprintf(stderr, "- "); }
            else if(it->IsBoundOff()) { for(int i = 0; i < it->takes; ++i) fprintf(stderr, "= "); }
            else {
                for(int i = 0; i < it->puts; ++i) fprintf(stderr, "| ");
            }
            ++it;
        }
        fprintf(stderr, "\t%c\t\t(@%d)\n", ll.reversed ? ' ' : '<', ll.srcLine);
    }
    LOG("");

    LOG("Instructions:");
    for(int i = 0; i < rows.size(); ++i)
    {
        auto& ll = rows[i];
        fprintf(stderr, "%3d: %c   ", ll.number, ll.reversed ? '>' : ' ');
        auto it = ll.First(!ll.reversed);
        while(it) {
            if(it->takes > it->puts) {
                int i = 0;
                for(; i < it->puts; ++i)  fprintf(stderr, "%s&", it->key);
                for(; i < it->takes; ++i) fprintf(stderr, ".%c", i < it->takes - 1 ? '&' : ' ');
            }
            else if(it->takes < it->puts) {
                int i = 0;
                for(; i < it->takes; ++i) fprintf(stderr, "%s&", it->key);
                for(; i < it->puts; ++i)  fprintf(stderr, "%s%c", it->key, i < it->puts - 1 ? '&' : ' ');
            }
            else if(it->IsSlipped()) { for(int i = 0; i < it->takes; ++i) fprintf(stderr, "%s ", it->key); }
            else if(it->IsBoundOff()) { for(int i = 0; i < it->takes; ++i) fprintf(stderr, "%s ", it->key); }
            else {
                for(int i = 0; i < it->puts; ++i) fprintf(stderr, "%s ", it->key);
            }
            ++it;
        }
        fprintf(stderr, "\t%c\t\t(@%d)\n", ll.reversed ? ' ' : '<', ll.srcLine);
    }
    LOG("");

    LOG("Populating graph");
    // iterate over row stitches starting with logical coordinates (0,0)
    // for each new produced stitch, increase X coord by incr*9
    // on new row, keep X, increase Y by 9; but check if we have any P stitches we need to pull up; add references on new row, and continue where left off
    // after than, apply forces and shift rows from [1..N)
    // after that, compute min & max, and move entire work such that min is (27,0)
    int xcoord = 0, ycoord = 0, dxcoord = 1;
    // preallocate memory because we need to keep pointers around
    std::vector<std::vector<Dot>> dots(rows.size(), std::vector<Dot>(longest));
    std::vector<int> numDots(rows.size());
    // iterator on previous row, for takes
    int previ = 0, previncr = +1;
    for(int rowy = 0; rowy < rows.size(); ++rowy)
    {
        // current row
        auto& row = rows[rowy];
        LOG("=============================");
        LOG("Populating row %d (%d@%d)", rowy, row.number, row.srcLine);
        LOG("=============================");
        int curri = 0; // where to insert the next dot; we can't allocate memory past this point because we'll be storing pointers to earlier points, and I don't want to deal with indirect memory references because that's silly
        // current row of dots where curri points into;
        // dots are always inserted at the end, since previ deals with the direction the stitches are moving in (previncr)
        auto& rowdots = dots[rowy];
        // is this current stitch connected to the previous one on the same row?
        bool disconnected = true;
        // iterator into actual stitching instructions;
        // the direction is never reversed, because the stitches were inserted in their natural order,
        // and we are following the natural order of things.
        auto it = row.First(false);
        LOG("Direction: %s", row.reversed ? "left-to-right" : "right-to-left");
        while(it) {
            LOG("-----------------------------------------------");
            LOG("Stitching at curri=%d (%d x %d) which is a %s", curri, xcoord, ycoord, it->key);
            LOG("        it at=%zd incr=%zd", it.at, it.incr);
            LOG("        xcoord = %d", xcoord);
            if(rowy > 0) {
                LOG("        The previous row has %d (or %d) stitches left, previ = %d", numDots[rowy - 1] - previ, previ, previ);
            }
            if(it->IsNormal() || it->IsBoundOff()) {
                LOG("    normal/boundoff");
                // generate it->puts stitches
                for(int ii = 0; ii < it->puts; ++ii) {
                    LOG("    putting %d at %d", ii, curri);
                    auto& dot = rowdots[curri++];
                    LOG("         curri now past the end at %d", curri);
                    dot.x = xcoord;
                    dot.y = ycoord;
                    dot.skip = false; // this stitch will participate in weight calculations
                    dot.stitchRef = &*it;
                    dot.dotRef = &dot;
                    // construct new stitches
                    if(!disconnected) {
                        // connect to previous stitch
                        if(        (ii == 0 && (it - 1)->IsNormal() /* first stitch of the chunk is connected to normal stitches unless someone threw in a disconnect (/) */)
                                || (ii == 0 && (curri - 2 >= 0) && (rowdots[curri - 2].stitchRef->IsBoundOff() || rowdots[curri - 2].stitchRef->IsNormal()) /* first stitch of the chunk may also be connected to bind-offs that generate a stitch (bo) or to whatever came before the a skip (sk) */)
                                || (ii > 0 /* subsequent stitches in this batch are connected to each other */)
                        ) {
                            LOG("...connecting to %d", curri - 2);
                            auto& prevDot = rowdots[curri - 2];
                            dot.lines.emplace_back();
                            dot.lines.back().dotRef = &prevDot;
                            dot.lines.back().otherDotRef = nullptr;
                            dot.lines.back().lineInfo = &connectingStitch;
                        }
                        else // previous stitch was a break or an unrenderable bind off, so we can't connect to it
                        {
                            disconnected = false;
                            dot.disconnected = true;
                            LOG("disconnected");
                        }
                    } else { // this is the first stitch on the line
                        disconnected = false;
                        dot.disconnected = true;
                        LOG("disconnected");
                    }

                    // update xcoord
                    xcoord = xcoord + 9 * dxcoord; // FIXME 9 is the size of the font, this should probably be refactored at some point...
                }

                // Handle '!' markers.
                // We need to translate "before"/"after" to "left"/"right" for the renderer
                if(it->markerBefore) {
                    LOG("    marker before");
                    if(row.reversed) {
                        LOG("          adding marker left on %d", curri - it->puts);
                        rowdots[curri - it->puts].markerLeft = true;
                    } else {
                        LOG("          adding marker right on %d", curri - it->puts);
                        rowdots[curri - it->puts].markerRight = true;
                    }
                }
                if(it->markerAfter) {
                    LOG("    marker after");
                    if(row.reversed) {
                        LOG("          adding marker right");
                        rowdots[curri - 1].markerRight = true;
                    } else {
                        LOG("          adding marker left");
                        rowdots[curri - 1].markerLeft = true;
                    }
                }

                // take stitches from previous row;
                // translate src and dst, keeping direction in mind
                if(it->takes > 9 ) { // be blessed ye who actually "k10tog" or "dc 10 into next sc"
                    LOG("Code only supports taking up to 9 stitches, asked to take %d", it->takes);
                    abort();
                }
                if(it->puts > 9 ) {
                    LOG("Code only supports putting up to 9 stitches, asked to put %d", it->puts);
                    abort();
                }
                // construct look-up tables from relative indices (0, 1, 2) to actual indices into the dots[] array
                int lutme[9]; // dst stitches
                int lutprev[9] = { previ - previncr, previ }; // src stitches
                memset(lutme, 255, 9 * sizeof(int));
                memset(lutprev + 2, 255, 7 * sizeof(int));
                // pre-initialize lutprev to previous and "current" stitch to handle MapEntry::INBETWEEN, 
                // which takes no stitches, but renders right after the previous stitch
                if(rowy > 0) {
                    while(lutprev[0] >= 0 && lutprev[0] < numDots[rowy - 1] && !dots[rowy - 1][lutprev[0]].stitchRef->IsCountable()) { lutprev[0] = lutprev[0] + previncr; lutprev[1] = lutprev[0] + previncr; }
                    while(lutprev[1] >= 0 && lutprev[1] < numDots[rowy - 1] && !dots[rowy - 1][lutprev[1]].stitchRef->IsCountable()) lutprev[1] = lutprev[1] + previncr;
                }
                // for the previous row, we advance previ in our iterator's direction
                for(int i = 0; i < it->takes; ++i) {
                    // skip boundoffs or skips or whatnot
                    while(previ >= 0 && previ < numDots[rowy - 1] && !dots[rowy - 1][previ].stitchRef->IsCountable()) previ = previ + previncr;
                    lutprev[i] = previ;
                    LOG("    taking %d/%d at previ %d", i, it->takes, previ);
                    previ = previ + previncr;
                }
                // I don't remember why I did this, probably for safety
                if(it->takes > 0) {
                    LOG("    adding one lutprev past-the-post");
                    lutprev[it->takes] = lutprev[it->takes - 1] + previncr;
                    if(rowy > 0) {
                        while(lutprev[it->takes] >= 0 && lutprev[it->takes] < numDots[rowy - 1] && !dots[rowy - 1][lutprev[it->takes]].stitchRef->IsCountable()) { lutprev[it->takes] = lutprev[it->takes] + previncr;}
                    }
                }
                // for the current row, we've just added the last "put" stitch, so take off
                // the last it->puts dots
                for(int i = 0; i < it->puts; ++i) {
                    lutme[i] = curri - it->puts + i;
                }
                LOG("    lutme: %d %d %d %d ...", lutme[0], lutme[1], lutme[2], lutme[3]);
                LOG("    lutprev: %d %d %d %d ...", lutprev[0], lutprev[1], lutprev[2], lutprev[3]);

                LOG("     .....");
                LOG("     Lining to previous row, per stitch map")
                // construct lines to previous row
                for(auto&& me : it->map) {
                    LOG("     line: %d <= %d", me.dst, me.src);
                    auto& dstDot = rowdots[lutme[me.dst]];
                    dstDot.lines.emplace_back();
                    dstDot.lines.back().lineInfo = &me;
                    if(me.src != MapEntry::INBETWEEN) {
                        dstDot.lines.back().dotRef = dots[rowy - 1][lutprev[me.src]].dotRef;
                        dstDot.lines.back().otherDotRef = nullptr;
                        LOG("         which is a %s", dstDot.lines.back().dotRef->stitchRef->key);
                    } else {
                        LOG("INBETWEEN");
                        dstDot.lines.back().dotRef = dots[rowy - 1][lutprev[0]].dotRef;
                        dstDot.lines.back().otherDotRef = dots[rowy - 1][lutprev[1]].dotRef;
                        dstDot.connectedTo.push_back(dstDot.lines.back().dotRef);
                        dstDot.connectedTo.push_back(dstDot.lines.back().otherDotRef);
                    }
                    LOG("     Stitching (%d,%d) -> (%d,%d)", rowy, lutme[me.dst], rowy-1, lutprev[me.src]);
                    LOG("    ....");
                }

                // save connections, because some stitches are technically "connected", but they
                // don't draw lines. This is for computing weights and aligning stuff
                LOG("     .....");
                LOG("     Connecting to previous row, per takes and puts");
                // connect to previous row
                for(int i = 0; i < it->puts; ++i) {
                    auto& dot = rowdots[lutme[i]];
                    for(int j = 0; j < it->takes; ++j) {
                        dot.connectedTo.push_back(dots[rowy - 1][lutprev[j]].dotRef);
                        LOG("Linking (%d,%d) -> (%d,%d)", rowy, lutme[i], rowy-1, lutprev[j]);
                    }
                }
                LOG("     .....");
            } else if(it->IsSlipped()) {
                LOG("    slipped/padding");
                // take from previous row
                for(int i = 0; i < it->takes; ++i) {
                    LOG("    taking %d at %d", i, curri);
                    if(curri >= rowdots.size()) {
                        LOG("curri = %d   rowdots.size() = %zd", curri, rowdots.size());
                        abort();
                    }
                    auto& dot = rowdots[curri++];
                    dot.x = xcoord;
                    dot.y = ycoord;
                    dot.skip = true; // these don't get rendered and don't affect layouting
                    dot.stitchRef = &*it;
                    dot.disconnected = true; // these are not connected horizontally yet;
                                             // technically they don't exist at all, you need
                                             // to refer directly to its dotRef to get to the
                                             // real stitch.
                    if(rowy > 0) {
                        // skip bindoffs or skips
                        while(!dots[rowy - 1][previ].stitchRef->IsCountable()) previ = previ + previncr;
                        dot.dotRef = dots[rowy - 1][previ].dotRef;
                        LOG("    connecting to previ %d", previ);
                        LOG("         which is a %s", dot.dotRef->stitchRef->key);
                        previ = previ + previncr;
                    } else {
                        dot.dotRef = &dot; // become one's own stitch if there were none;
                                           // this will probably break later, you ought to
                                           // cast on enough stitches on row 0
                    }

                    // update xcoord
                    xcoord = xcoord + 9 * dxcoord;
                }
            } else if(it->IsBroken()) {
                // special stitch to create 2+ disconnected regions in the pattern
                LOG("broken, resetting 'disconnected'");
                disconnected = true; // I guess we're done here?
            } // case normal/boundoff, slipped, broken
            // advance stitching instructions
            ++it;
        } // while(it)

        // save how many dots we have on this row; recall we can't reallocate memory
        // because of pointers; and I don't want to use shared pointers or linked lists
        numDots[rowy] = curri;

        // update previous row iterator;
        if(rowy < rows.size() - 1) {
            if(rows[rowy].turn) {
                LOG("we turn!");
                previ = curri - 1;
                previncr = -1;
                // skip bindoffs
                while(previ >= 0 && !rowdots[previ].stitchRef->IsCountable()) --previ;
                xcoord = dots[rowy][numDots[rowy] - 1].x;
                dxcoord = -dxcoord;
            } else {
                LOG("we go around!");
                xcoord = 0;
                previ = 0;
                previncr = +1;
                // skip bindoffs
                while(previ < curri && !rowdots[previ].stitchRef->IsCountable()) ++previ;
            }
            LOG("previ reset to %d %+d; xcoord is %d", previ,previncr, xcoord);
            LOG("next row direction: %s", rows[rowy + 1].reversed ? "left-to-right" : "right-to-left");
        }

        // move to next row
        ycoord += 9; // FIXME refactor font size out
    }
    LOG("");
    LOG("dots = ");
    for(int rowy = 0; rowy < dots.size(); ++rowy) {
        for(int rowx = 0; rowx < numDots[rowy]; ++rowx) {
            auto& dot = dots[rowy][rowx];
            fprintf(stderr, "(%3d,%3d,%3d,%d) ", dot.x, dot.y, int(dot.lines.size()), dot.stitchRef->special);
        }
        fprintf(stderr, "\n");
    }

    LOG("Computing layout");
    for(int rowy = 1; rowy < dots.size(); ++rowy) {
        LOG("========================");
        LOG("Processing row %d (%d@%d)", rowy, rows[rowy].number, rows[rowy].srcLine);
        // group continuous (unbroken) stitches
        int group_start = 0, group_end = 0, group_num = 0;
        while(group_end < numDots[rowy])
        {
            ++group_num;
            group_end = group_start + 1;
            LOG("group_end = %d", group_end);
            while(group_end < numDots[rowy] && !dots[rowy][group_end].dotRef->disconnected) ++group_end;

            LOG("-------------------")
            LOG("Group %d from %d to %d (out of %d)", group_num, group_start, group_end, numDots[rowy]);

            float force = 0.f;
            int denominator = 0;
            for(int rowx = group_start; rowx < group_end; ++rowx) {
                if(dots[rowy][rowx].skip) {
                    continue;
                }
                float lforce = 0.f;
                int ldenom = 0;
                auto& self = dots[rowy][rowx];
                float myX = self.x;
                auto lineit = self.connectedTo.begin();
                for(; lineit != self.connectedTo.end(); ++lineit)
                {
                    //fprintf(stderr, "%d... ", (*lineit)->x);
                    lforce += (*lineit)->x;
                    ldenom += 1;
                }
                if(ldenom == 0) continue;
                force += lforce / ldenom - myX;
                denominator += 1.f;
                //fprintf(stderr, "%f ", lforce/ldenom - myX);
            }
            if(denominator > 0)
            {
                force = force / denominator;
                //fprintf(stderr, "\n");
                int adj = int(round(force));
                LOG("Force on line %d (%d@%d) is %f ~=%d", rowy, rows[rowy].number, rows[rowy].srcLine, force, adj);
                for(int rowx = group_start; rowx < group_end; ++rowx) {
                    //LOG("%d -> %d", dots[rowy][rowx].x, dots[rowy][rowx].x + adj);
                    dots[rowy][rowx].x += adj;
                }
            }

            // to next group!
            group_start = group_end;
            LOG("group_start = %d", group_start);
        }
    }
    LOG("--------------------");
    LOG("dots = ");
    for(int rowy = 0; rowy < dots.size(); ++rowy) {
        for(int rowx = 0; rowx < numDots[rowy]; ++rowx) {
            auto& dot = dots[rowy][rowx];
            fprintf(stderr, "(%3d,%3d,%3d,%d) ", dot.x, dot.y, int(dot.lines.size()), dot.stitchRef->special);
        }
        fprintf(stderr, "\n");
    }
    LOG("");

    LOG("Adjusting virtual coordinates to PNG canvas");
    int minx = 32767, maxx = -32768;
    for(int rowy = 0; rowy < dots.size(); ++rowy) {
        for(int rowx = 0; rowx < numDots[rowy]; ++rowx) {
            auto& dot = dots[rowy][rowx];
            if(dot.x < minx) minx = dot.x;
            if(dot.x > maxx) maxx = dot.x;
        }
    }
    LOG("Coordinates go from %d to %d", minx, maxx);
    int canvasHeight = (rows.size() + 2) * 9;
    for(int rowy = 0; rowy < dots.size(); ++rowy) {
        for(int rowx = 0; rowx < dots[rowy].size(); ++rowx) {
            auto& dot = dots[rowy][rowx];
            dot.x = dot.x - minx + 3 * 9 /*row number*/ + 9 /* whitespace padding */;
            dot.y = canvasHeight - 9 - dot.y;
        }
    }
    LOG("dots = ");
    for(int rowy = 0; rowy < dots.size(); ++rowy) {
        for(int rowx = 0; rowx < numDots[rowy]; ++rowx) {
            auto& dot = dots[rowy][rowx];
            fprintf(stderr, "(%3d,%3d,%3d,%d) ", dot.x, dot.y, int(dot.lines.size()), dot.stitchRef->special);
        }
        fprintf(stderr, "\n");
    }
    // number of pixels our chart takes horizontally
    int dotsExtent = maxx - minx;

    int canvasWidth = 3 * 9 /* 3 digits row number, reversed direction */
                    + 9 /* whitespace padding */
                    + dotsExtent /* longest row */ 
                    + 9 /* whitespace padding */
                    + 3 * 9 /* 3 digits row number, forward direction */
                    + 5 * 9 /* space for stitch count, 2 parens, 3 digits */
                    ;
    LOG("Populating canvas %d x %d", canvasWidth, canvasHeight);
    auto hcanvas = initCanvas(canvasWidth, canvasHeight);

    // render everything; technically rendering is deferred such that
    // markers are on top of lines on top of glyphs
    for(int rowy = 0; rowy < dots.size(); ++rowy) {
        // row counter
        // you'd need to be mad to have more than 999
        std::string rown = std::to_string(rows[rowy].number);
        for(int z = 0; rows[rowy].number > 0 && z < rown.size(); ++z) {
            int x = 0;
            if(!rows[rowy].reversed) {
                x = (rown.size() - 1 - z) * 9;
                x += dotsExtent + 3 * 9 + 9 + 9;
            } else {
                x = (2 - z) * 9;
            }
            drawGlyph(hcanvas, rown[rown.size() - 1 - z], BLACK, x, (2 + rows.size() - 1 - rowy) * 9 - 3);
        }

        // stitch count
        // skip if 0
        auto nstcnt = rows[rowy].CountNormal();
        std::string stcnt = std::to_string(nstcnt);
        if(nstcnt > 0) {
            // you'd need to be mad to have more than 999
            drawGlyph(hcanvas, '(', BLACK, 3 * 9 + 9 + dotsExtent + 9 + 3 * 9 + 9 + (2 - stcnt.size()) * 9, (2 + rows.size() - 1 - rowy) * 9 - 3);
            for(int z = 0; z < stcnt.size(); ++z) {
                int x = 0;
                x = 3 * 9 + 9 + dotsExtent + 9 + 3 * 9 + 9 + 9 + z * 9 + (2 - stcnt.size()) * 9;
                drawGlyph(hcanvas, stcnt[z], BLACK, x, (2 + rows.size() - 1 - rowy) * 9 - 3);
            }
            drawGlyph(hcanvas, ')', BLACK, 3 * 9 + 9 + dotsExtent + 9 + 3 * 9 + 4 * 9, (2 + rows.size() - 1 - rowy) * 9 - 3);
        }

        // stitches proper
        for(int rowx = 0; rowx < numDots[rowy]; ++rowx) {
            auto& dot = dots[rowy][rowx];
            // stitch knot itself
            if(!dot.skip && dot.stitchRef && dot.stitchRef->marker != NONE) {
                drawMarker(hcanvas, dot.stitchRef->marker, dot.stitchRef->color, dot.x, dot.y);
            }
            // EXCLAMATION markers, if any
            if(dot.markerLeft) {
                LOG("Marker at %d x %d", dot.x - 4, dot.y);
                drawMarker(hcanvas, EXCLAMATION, RED, dot.x - 4, dot.y);
            }
            if(dot.markerRight) {
                LOG("Marker at %d x %d", dot.x + 5, dot.y);
                drawMarker(hcanvas, EXCLAMATION, RED, dot.x + 5, dot.y);
            }
            // connections
            for(auto&& lspec : dot.lines) {
                int tx = lspec.dotRef->x;
                int ty = lspec.dotRef->y;
                if(lspec.otherDotRef != nullptr) {
                    // MapEntry::INBETWEEN, we render inbetween two dots
                    tx = (lspec.dotRef->x + lspec.otherDotRef->x) / 2;
                    ty = (lspec.dotRef->y + lspec.otherDotRef->y) / 2;
                }
                drawLine(hcanvas, lspec.lineInfo->color, dot.x, dot.y, tx, ty);
                // does the line have a marker?
                if(lspec.lineInfo->marker != NONE) {
                    drawMarker(hcanvas, lspec.lineInfo->marker, lspec.lineInfo->color, (dot.x + tx) / 2, (dot.y + ty) / 2);
                }
            }
        }
    }
    
    // write out bitmap
    auto graphFname = fname + ".png"s;
    LOG("Writing %s", graphFname.c_str());
    // technically the canvas only gets rendered here
    writeCanvas(hcanvas, graphFname.c_str());
    destroyCanvas(hcanvas);

    for(size_t i = 0; i < dots.size(); ++i) {
        dots[i].resize(numDots[i]);
    }

    // write out cylinder
    LOG("Writing %s.obj", fname.c_str());
    extern void WriteCylinder(std::string const&, std::vector<std::vector<Dot>>&, int, int, int, int, int, int, float);
    WriteCylinder(fname, dots, 3*9 + 9, 9 /* never any downward lines on first row*/, 3*9+9+dotsExtent, canvasHeight /* 9px of padding at top */, 3*9+9, canvasWidth, 2 * 3.14159);
}

static const char* FLAGS = "h"; // so many options!
void usage(const char* argv0)
{
    printf("%s", reinterpret_cast<const char*>(u8"StitchGraph by Vlad Meșco\n\n"));
    printf("Usage: %s [-%s] file ...\n", argv0, FLAGS);
    exit(2);
}
int main(int argc, char* argv[])
{
    int opt;
    char* argv0 = strdup(argv[0]);

    while((opt = getopt(argc, argv, FLAGS)) != -1) {
        switch(opt) {
            case 'h':
                usage(argv0);
                break;
            default:
                fprintf(stderr, "No such switch: %c\n", opt);
                break;
        }
    }

    if(optind >= argc) usage(argv0);
    free(argv0);

    for(int argvi = optind; argvi < argc; ++argvi) {
        std::string fname = argv[argvi];
        printf("Processing %s\n", fname.c_str());
        process_stitch_file(fname);
    }

    // done-dee-done.
    return 0;
}
