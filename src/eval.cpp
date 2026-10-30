#include<iostream>
#include "board.h"
#include "pst.h"

int evaluate(const Board &b)
{
    int cost=0;
    for(int sq=0;sq<NUM_SQUARES;sq++)
    {
        int idx=((b.pieceAt(sq)&COLOR_MASK) == WHITE) ? sq^56 : sq; 
        if((b.pieceAt(sq)&COLOR_MASK) == b.getSidetoMove()) cost+=PIECE_VALUE[(b.pieceAt(sq)&TYPE_MASK)] + PST[(b.pieceAt(sq)&TYPE_MASK)][idx];
        else cost-=(PIECE_VALUE[(b.pieceAt(sq)&TYPE_MASK)] + PST[(b.pieceAt(sq)&TYPE_MASK)][idx]);
    }
    return cost;
}