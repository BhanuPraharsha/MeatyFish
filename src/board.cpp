#include<iostream>
#include<cctype>
#include<sstream>
#include "board.h"
#include "move.h"


static char num2piece(int square)
{
    char piece='.';
    int pieceVal=TYPE_MASK & square;
    if(pieceVal == QUEEN) piece='q';
    else if(pieceVal == ROOK) piece='r';
    else if(pieceVal == BISHOP) piece='b';
    else if(pieceVal == KNIGHT) piece='n';
    else if(pieceVal == KING) piece='k';
    else if(pieceVal == PAWN) piece='p';

    if((COLOR_MASK & square) == WHITE) piece = std::toupper(piece);
    return piece;
}

static int piece2num(char piece)
{
    int color = std::isupper(piece) ? WHITE : BLACK;
    switch(std::tolower(piece))
    {
        case 'q': return color | QUEEN;
        case 'r': return color | ROOK;
        case 'b': return color | BISHOP;
        case 'n': return color | KNIGHT;
        case 'k': return color | KING;
        case 'p': return color | PAWN;
    }
    return NONE;
}

static int castlingMask(int sq)
{
    switch (sq)
    {
    case E1: return ~(WHITE_KINGSIDE | WHITE_QUEENSIDE);
    case H1: return ~WHITE_KINGSIDE;
    case A1: return ~WHITE_QUEENSIDE;
    case E8: return ~(BLACK_KINGSIDE | BLACK_QUEENSIDE);
    case H8: return ~BLACK_KINGSIDE;
    case A8: return ~BLACK_QUEENSIDE;
    default: return ~0;               // all bits set: changes nothing
    }
}


void Board::clear()
{
    for(int j=0;j<NUM_SQUARES;j++)
    {
        squares[j]=NONE;
    }
    sideToMove = WHITE;
    castlingRights = 0;
    enPassantSquare = NO_SQUARE;
    halfMoveClock = 0;
    fullMoveClock = 1;
    return;
}

void Board::printBoard() const
{
    for(int row=WIDTH-1;row>=0;row--)
    {
        for(int col=0;col<WIDTH;col++)
        {
            std::cout<<num2piece(squares[row*WIDTH + col])<<" ";
        }
        std::cout<<"\n";
    }
    return;
}


void Board::loadFEN(const std::string& fen)
{
    clear();
    std::istringstream inp(fen);
    std::string board, turn, castling, enpassant;
    inp>>board>>turn>>castling>>enpassant>>halfMoveClock>>fullMoveClock;

    // field 1: piece placement, rank 8 first
    int row=WIDTH-1, col=0;
    for(char c : board)
    {
        if(c=='/') row--, col=0;
        else if(c>='1' and c<='8') col+=(c-'0');
        else
        {
            squares[row*WIDTH + col]=piece2num(c);
            col++;
        }
    }

    // field 2: side to move
    sideToMove = (turn=="w") ? WHITE : BLACK;

    // field 3: castling rights 
    for(char c : castling)
    {
        if(c=='K') castlingRights |= WHITE_KINGSIDE;
        else if(c=='Q') castlingRights |= WHITE_QUEENSIDE;
        else if(c=='k') castlingRights |= BLACK_KINGSIDE;
        else if(c=='q') castlingRights |= BLACK_QUEENSIDE;
    }

    // field 4: en passant target square 
    if(enpassant != "-")
    {
        int col = enpassant[0]-'a';
        int row = enpassant[1]-'1';
        enPassantSquare = row*WIDTH + col;
    }
}


void Board::makeMove(const Move& m)
{
    Undo change;
    change.captured=squares[m.to];
    change.castlingRights=castlingRights;
    change.enPassantSquare=enPassantSquare;
    change.halfMoveClock=halfMoveClock;
    change.move=m;

    if(m.enPassant)
    {
        //remove the pawn
        squares[boardPos(rowOf(m.from), columnOf(m.to))]=NONE;
    }


    /* castling rights updation*/
    castlingRights &= castlingMask(m.from) & castlingMask(m.to); // handles cases when getting captured/moving 

    squares[m.to]=squares[m.from];

    // update halfMoveClock and fullMoveClock
    if(((change.captured & TYPE_MASK) != NONE) || (squares[m.to] & TYPE_MASK)==PAWN)
    {
        //reset counter for the 50move rule
        halfMoveClock=0;
    }
    else{
        halfMoveClock++;
    }
    
    if(m.promotion) squares[m.to]=(squares[m.to]&COLOR_MASK) | m.promotion; // promotion
    squares[m.from]=NONE;
    sideToMove^=COLOR_MASK;

    if((squares[m.to] & COLOR_MASK) == BLACK) fullMoveClock++;

    enPassantSquare=NO_SQUARE;
    if(((squares[m.to] & TYPE_MASK) == PAWN) && (abs(m.to - m.from) == 2*WIDTH))
    {
        enPassantSquare = (m.to + m.from)/2; //unconditionaly mark the en-passant square
    }

    changeHistory.push_back(change);
    return;
}

void Board::unmakeMove()
{
    Undo prevChange=changeHistory.back();
    castlingRights=prevChange.castlingRights;
    enPassantSquare=prevChange.enPassantSquare;
    halfMoveClock=prevChange.halfMoveClock;
    sideToMove^=COLOR_MASK;
    squares[prevChange.move.from]=squares[prevChange.move.to];
    if(prevChange.move.promotion) //it was a promotional move
    {
        squares[prevChange.move.from] = (squares[prevChange.move.from]&COLOR_MASK) | PAWN;
    }
    squares[prevChange.move.to]=prevChange.captured;

    if(prevChange.move.enPassant)
    {
        //pawn replacement
        int pawn=(squares[prevChange.move.from] ^ COLOR_MASK) | PAWN;
        squares[boardPos(rowOf(prevChange.move.from), columnOf(prevChange.move.to))]=pawn;
    }

    if((squares[prevChange.move.from] & COLOR_MASK) == BLACK) fullMoveClock--;
    changeHistory.pop_back();
    return;
}