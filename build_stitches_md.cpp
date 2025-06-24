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

void PrintMap(FILE* f, std::vector<MapEntry> const& map)
{
    if(map.empty()) return;
    fprintf(f, "- map: ");
    for(auto&& me : map) {
        fprintf(f, "(%d <- %d) ", me.dst, me.src);
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
