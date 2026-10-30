#pragma once    
#include "board.h"

Move bestMove(Board &b, int depth, long long &nodesVis);
int benchmark(Board &b, int depth, bool prune, long long &nodes);