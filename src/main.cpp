#include<iostream>
using namespace std;
#include "board.h"


int main()
{
    Board b;
    b.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    b.printBoard();
    return 0;
}