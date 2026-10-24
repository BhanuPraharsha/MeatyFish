#include<iostream>
#include<vector>
#include "move.h"
#include "board.h"
#include "types.h"
#include "movegen.h"

struct Dir { int dRow; int dCol; };

static constexpr Dir ROOK_DIRS[]   = { {1,0}, {-1,0}, {0,1}, {0,-1} };
static constexpr Dir BISHOP_DIRS[] = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };
static constexpr Dir QUEEN_DIRS[]  = { {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1} };
static constexpr Dir KING_DIRS[]   = { {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1} };
static constexpr Dir KNIGHT_DIRS[] = { {1,2}, {1,-2}, {-1,2}, {-1,-2}, {2,1}, {2,-1}, {-2,1}, {-2,-1} };

static void addPawnMoves(const Board& b, int from, std::vector<Move>&moves)
{
    int dir=((b.pieceAt(from) & COLOR_MASK) == WHITE)? +1: -1;
    int color=b.pieceAt(from) & COLOR_MASK;
    int curRow=rowOf(from), curCol=columnOf(from);
    int startRow = (color == WHITE) ? 1 : 6;

    int r=curRow + dir, c=curCol;
    bool df = (r >= 0 && r < WIDTH && b.pieceAt(boardPos(r, c)) == NONE);

    if(df) moves.push_back({from, boardPos(r, c)});
    r+=dir;

    if(r>=0 && r<WIDTH && c>=0 && c<WIDTH && curRow==startRow && df && b.pieceAt(boardPos(r, c)) == NONE)
    {
        moves.push_back({from, boardPos(r, c)}); // double push
    }

    r-=dir;

    c+=1;
    //check captures
    if(r>=0 && r<WIDTH && c>=0 && c<WIDTH &&
        ((b.pieceAt(boardPos(r,c))&COLOR_MASK)!=(b.pieceAt(boardPos(curRow,curCol))&COLOR_MASK)) && b.pieceAt(boardPos(r,c)) != NONE) moves.push_back({from, boardPos(r, c)}); 
    c-=2;
    if(r>=0 && r<WIDTH && c>=0 && c<WIDTH &&
        ((b.pieceAt(boardPos(r,c))&COLOR_MASK)!=(b.pieceAt(boardPos(curRow,curCol))&COLOR_MASK)) && b.pieceAt(boardPos(r,c)) != NONE) moves.push_back({from, boardPos(r, c)});

    return;
}


static void addMoves(const Board& b, int from, const Dir* dirs, int numDirs, int maxSteps,  std::vector<Move>& moves)
{
    int curRow=rowOf(from), curCol=columnOf(from);
    for(int j=0;j<numDirs;j++)
    {
        for(int stepCount=1;stepCount<=maxSteps;stepCount++)
        {
            int r=curRow + dirs[j].dRow * stepCount, c=curCol + dirs[j].dCol * stepCount;
            if(r<0 || r>=WIDTH || c<0 || c>=WIDTH) continue;
            int target=b.pieceAt(boardPos(r, c));
            if((target & COLOR_MASK) == (b.pieceAt(from) & COLOR_MASK)) break; //obstruction
            if(target != NONE)
            {
                //opponent piece occupied
                moves.push_back({from, boardPos(r, c)});
                break;
            }
            moves.push_back({from, boardPos(r, c)}); //empty spot
        }
    }


    //need to handle enpassant, castling later
    return;
}


std::vector<Move> generatePseudoMoves(const Board& b)
{
    std::vector<Move> moves;
    for(int sq=0;sq<NUM_SQUARES;sq++)
    {
        int piece=b.pieceAt(sq);

        if(piece==NONE)  continue;
        if((piece & COLOR_MASK)!=b.getSidetoMove()) continue;

        int pieceType=(piece & TYPE_MASK);
        const Dir* dirs=nullptr;
        int numDirs=0, maxSteps=0;

        switch (pieceType)
        {
        case QUEEN:
            dirs=QUEEN_DIRS;
            numDirs=8;
            maxSteps=7;
            break;
        case ROOK:
            dirs=ROOK_DIRS;
            numDirs=4;
            maxSteps=7;
            break;
        case BISHOP:
            dirs=BISHOP_DIRS;
            numDirs=4;
            maxSteps=7;
            break;
        case KNIGHT:
            dirs=KNIGHT_DIRS;
            numDirs=8;
            maxSteps=1;
            break;
        case KING:
            dirs=KING_DIRS;
            numDirs=8;
            maxSteps=1;
            break;
        case PAWN:
            //call the pawn handler
            addPawnMoves(b, sq, moves);
            continue;
        }
        addMoves(b, sq, dirs, numDirs, maxSteps, moves);
    }

    return moves;
}