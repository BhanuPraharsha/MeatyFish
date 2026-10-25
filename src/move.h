#pragma once
#include <string>
#include "types.h"

struct Move{
    int from;
    int to;
    int promotion=NONE;
    bool castle=false;
    bool enPassant=false;
};

std::string move2string(const Move &m);
