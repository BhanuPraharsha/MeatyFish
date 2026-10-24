#include<iostream>
#include<cctype>
#include<sstream>
#include "board.h"


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
