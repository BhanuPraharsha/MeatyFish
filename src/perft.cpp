#include<iostream>
#include "perft.h"
#include "movegen.h"
#include <string>


long perft(Board &b, int depth)
{
    if(depth==0) return 1;
    long count=0;
    for(auto move : generateLegalMoves(b))
    {
        b.makeMove(move);
        count += perft(b, depth-1);
        b.unmakeMove();
    }

    return count;
}

long divide(Board &b, int depth, bool isRoot)
{
    if(depth==0) return 1;
    long count=0;
    for(auto move : generateLegalMoves(b))
    {
        b.makeMove(move);
        long curr=divide(b, depth-1, false);
        if(isRoot) std::cout<<move2string(move)<<": "<<curr<<std::endl;
        count += curr;
        b.unmakeMove();
    }
    return count;
}