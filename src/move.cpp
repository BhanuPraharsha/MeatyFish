#include "move.h"

std::string move2string(const Move &m)
{
    std::string s;
    s+=char('a' + columnOf(m.from));
    s+=char('1' + (rowOf(m.from)));
    s+=char('a' + columnOf(m.to));
    s+=char('1' + (rowOf(m.to)));

    if(m.promotion!=NONE)
    {
        if(m.promotion==QUEEN) s+='q';
        else if(m.promotion==KNIGHT) s+='n';
        else if(m.promotion==ROOK) s+='r';
        else if(m.promotion==BISHOP) s+='b';
    }
    return s;
}
