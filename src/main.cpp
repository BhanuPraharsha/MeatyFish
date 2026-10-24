#include<iostream>
using namespace std;
#include "board.h"
#include "movegen.h"


int main()
{
    Board b;
    b.loadFEN("8/8/8/3pP3/8/8/8/8 w - d6 0 1");
    b.printBoard();
    std::cout<<endl;

    Move m({E5, D6});
    m.enPassant=true;

    b.makeMove(m);
    b.printBoard();
    std::cout<<endl;

    b.unmakeMove();
    b.printBoard();
    std::cout<<endl;


    std::cout << endl;
    for (const Move &m : generatePseudoMoves(b))
        cout << char('a' + columnOf(m.from)) << rowOf(m.from) + 1
             << char('a' + columnOf(m.to)) << rowOf(m.to) + 1 << "\n";

    cout<<generatePseudoMoves(b).size()<<endl;

    return 0;
}