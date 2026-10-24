#pragma once
#include<string>
#include <vector>
#include "types.h"
#include "move.h"

struct Undo{
    Move move;
    int captured;
    int castlingRights;
    int enPassantSquare;
    int halfMoveClock;
};

class Board{
public:
    void clear();
    void loadFEN(const std::string &fen);
    void printBoard() const;
    int pieceAt(int sq) const {return squares[sq];}
    int getSidetoMove() const {return sideToMove;}
    void makeMove(const Move& m);
    void unmakeMove();
    int getEnpassantSquare() const {return enPassantSquare;}

private:
    int squares[NUM_SQUARES] = {};
    int sideToMove = WHITE;
    int castlingRights = 0;
    int enPassantSquare = NO_SQUARE;
    /* for the 50 move logic */
    int halfMoveClock = 0;
    int fullMoveClock = 1;

    std::vector<Undo> changeHistory;
};