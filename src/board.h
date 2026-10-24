#pragma once
#include<string>
#include "types.h"

class Board{
public:
    void clear();
    void loadFEN(const std::string &fen);
    void printBoard() const;

private:
    int squares[NUM_SQUARES] = {};
    int sideToMove = WHITE;
    int castlingRights = 0;
    int enPassantSquare = NO_SQUARE;
    /* for the 50 move logic */
    int halfMoveClock = 0;
    int fullMoveClock = 1;
};