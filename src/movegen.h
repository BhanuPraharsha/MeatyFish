#pragma once
#include<vector>
#include "board.h"

std::vector<Move> generatePseudoMoves(const Board& b);
std::vector<Move> generateLegalMoves(Board& b);
bool isSquareAttacked(const Board& b, int square, int byColor);
Move string2move(std::string& s, Board &b);
int kingSquare(const Board& b);