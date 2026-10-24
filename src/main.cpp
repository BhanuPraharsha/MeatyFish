#include<iostream>
using namespace std;
#include "board.h"
#include "movegen.h"


int main()
{
    Board b;
    b.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    b.printBoard();
    std::cout << endl;
    for (const Move &m : generatePseudoMoves(b))
        cout << char('a' + columnOf(m.from)) << rowOf(m.from) + 1
             << char('a' + columnOf(m.to)) << rowOf(m.to) + 1 << "\n";

    cout<<generatePseudoMoves(b).size()<<endl;

    return 0;
}