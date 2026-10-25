#pragma once
#include "board.h"

long perft(Board &b, int depth);
long divide(Board &b, int depth, bool isRoot);