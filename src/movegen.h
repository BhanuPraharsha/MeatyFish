#pragma once
#include<vector>
#include "board.h"
#include "move.h"

std::vector<Move> generatePseudoMoves(const Board& b);
bool isSquareAttacked(const Board& b, int square, int byColor);
