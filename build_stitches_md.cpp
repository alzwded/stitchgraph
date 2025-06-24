/*
Copyright 2025 Vlad Meșco

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "stitches.h"
#include <cstdio>

const char* TranslateMarker(Marker m)
{
#define C(X) case X: return #X
    switch(m)
    {
        default:
        C(NONE);
        C(DOT);
        C(CIRCLE);
        C(SQUIGGLE);
        C(ARROWDOWN);
        C(ARROWUP);
        C(EXCLAMATION);
        C(HBAR);
        C(H2BAR);
        C(H3BAR);
        C(VBAR);
        C(ARROWLEFT);
        C(ARROWRIGHT);
        C(BOBBLE);
    }
#undef C
}

void printSource(FILE* f, int src)
{
    if(src != MapEntry::INBETWEEN) fprintf(f, "%d", src);
    else fprintf(f, "inbetween");
}

void PrintMap(FILE* f, std::vector<MapEntry> const& map)
{
    if(map.empty()) return;
    fprintf(f, "- map: ");
    for(auto&& me : map) {
        fprintf(f, "(%d <- ", me.dst);
        printSource(f, me.src);
        fprintf(f, ") ");
    }
    fprintf(f, "\n");
}

int main()
{
    FILE* f = fopen("Stitches.md", "w");

    for(auto&& st : STITCHES) {
        fprintf(f, "## `%s`\n\n", st.key);
        const char* special = "";
        if(st.special == Stitch::P) special = "(special slip/blank stitch)";
        else if(st.special == Stitch::BREAK) special = "(special broken stitch to allow multiple skeins or creating big holes)";
        else if(st.special == Stitch::BINDOFF) special = "(bind off stitch)";
        fprintf(f, "- Symbol: `%s`\n- Description: %s %s\n", st.key, st.description, special);
        fprintf(f, "- takes: %d\n", st.takes);
        fprintf(f, "- puts:  %d\n", st.puts);
        if(st.marker != NONE)
        {
            fprintf(f, "- marker: %s\n", TranslateMarker(st.marker));
        }
        PrintMap(f, st.map);
        fprintf(f, "\n");
    }

    fclose(f);
    return 0;
}
