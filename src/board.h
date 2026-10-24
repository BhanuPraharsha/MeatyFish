#pragma once
#include<string>
#include "types.h"

class Board{
public:
    void clear();
    void loadFEN(const std::string &fen);
    void printBoard() const;
    int pieceAt(int sq) const {return squares[sq];}
    int getSidetoMove() const {return sideToMove;}

private:
    int squares[NUM_SQUARES] = {};
    int sideToMove = WHITE;
    int castlingRights = 0;
    int enPassantSquare = NO_SQUARE;
    /* for the 50 move logic */
    int halfMoveClock = 0;
    int fullMoveClock = 1;
};