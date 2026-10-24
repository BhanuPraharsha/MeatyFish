#pragma once
#include "types.h"

struct Move{
    int from;
    int to;
    int promotion=NONE;
    bool castle=false;
    bool enPassant=false;
};



