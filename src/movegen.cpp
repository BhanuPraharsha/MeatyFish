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

static void addPawnMove(int from, int to, std::vector<Move>& moves)
{
    int row = rowOf(to);
    if(row == 0 || row == WIDTH-1)
        for (int p : {QUEEN, ROOK, BISHOP, KNIGHT}) moves.push_back({from, to, p});
    else
        moves.push_back({from, to});
}

static bool onBoard(int r, int c)
{
    return r >= 0 && r < WIDTH && c >= 0 && c < WIDTH;
}

static bool hasPiece(const Board& b, int r, int c, int byColor, int type)
{
    if(!onBoard(r, c)) return false;
    return ((b.pieceAt(boardPos(r, c)) & COLOR_MASK)==byColor && (b.pieceAt(boardPos(r, c)) & TYPE_MASK)==type);
}

static void addPawnMoves(const Board& b, int from, std::vector<Move>&moves)
{
    int dir=((b.pieceAt(from) & COLOR_MASK) == WHITE)? +1: -1;
    int color=b.pieceAt(from) & COLOR_MASK;
    int curRow=rowOf(from), curCol=columnOf(from);
    int startRow = (color == WHITE) ? 1 : 6;

    int r=curRow + dir, c=curCol;
    bool df = (r >= 0 && r < WIDTH && b.pieceAt(boardPos(r, c)) == NONE);

    if(df)
    {
        addPawnMove(from, boardPos(r, c), moves);
    }
    r+=dir;

    if(r>=0 && r<WIDTH && c>=0 && c<WIDTH && curRow==startRow && df && b.pieceAt(boardPos(r, c)) == NONE)
    {
        moves.push_back({from, boardPos(r, c)}); // double push
    }

    r-=dir;

    //check captures and en-passants
    for (int dc : {-1, 1})
    {
        c += dc;
        if (r >= 0 && r < WIDTH && c >= 0 && c < WIDTH && ((b.pieceAt(boardPos(r, c)) & COLOR_MASK) != (b.pieceAt(boardPos(curRow, curCol)) & COLOR_MASK)) && b.pieceAt(boardPos(r, c)) != NONE)
        {
            addPawnMove(from, boardPos(r, c), moves);
        }
        if (r >= 0 && r < WIDTH && c >= 0 && c < WIDTH && b.getEnpassantSquare() == boardPos(r, c))
        {
            Move m({from, boardPos(r, c)});
            m.enPassant = true;
            moves.push_back(m);
        }
        c -= dc;
    }

    return;
}


static void addMoves(const Board& b, int from, const Dir* dirs, int numDirs, int maxSteps,  std::vector<Move>& moves)
{
    int curRow = rowOf(from), curCol = columnOf(from);
    for (int j = 0; j < numDirs; j++)
    {
        for (int stepCount = 1; stepCount <= maxSteps; stepCount++)
        {
            int r = curRow + dirs[j].dRow * stepCount, c = curCol + dirs[j].dCol * stepCount;
            if (r < 0 || r >= WIDTH || c < 0 || c >= WIDTH)
                continue;
            int target = b.pieceAt(boardPos(r, c));
            if ((target & COLOR_MASK) == (b.pieceAt(from) & COLOR_MASK))
                break; // obstruction
            if (target != NONE)
            {
                // opponent piece occupied
                moves.push_back({from, boardPos(r, c)});
                break;
            }
            moves.push_back({from, boardPos(r, c)}); // empty spot
        }
    }

    // need to handle enpassant, castling later
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

static bool attackedByStepper(const Board& b, int r, int c, int byColor, int type, const Dir* dirs, int numDirs)
{
    for(int j=0;j<numDirs;j++)
    {
        if(hasPiece(b, r + dirs[j].dRow, c + dirs[j].dCol, byColor, type)) return true;
    }
    return false;
}

static bool attackedBySlider(const Board& b, int r, int c, int byColor, int type, const Dir* dirs, int numDirs)
{
    for(int j=0;j<numDirs;j++)
    {
        for(int step=1;step<WIDTH;step++)
        {
            int nr=r + dirs[j].dRow * step, nc=c + dirs[j].dCol * step;
            if(!onBoard(nr, nc)) break;
            if(b.pieceAt(boardPos(nr, nc)) == NONE) continue;
            if(hasPiece(b, nr, nc, byColor, type) || hasPiece(b, nr, nc, byColor, QUEEN)) return true;
            break;
        }
    }
    return false;
}

static bool attackedByPawn(const Board& b, int r, int c, int byColor)
{
    int pawnRow=(byColor == WHITE) ? r - 1 : r + 1;
    for(int dc : {-1, 1})
    {
        if(hasPiece(b, pawnRow, c + dc, byColor, PAWN)) return true;
    }
    return false;
}

bool isSquareAttacked(const Board& b, int square, int byColor)
{
    int r = rowOf(square), c = columnOf(square);
    return attackedByStepper(b, r, c, byColor, KNIGHT, KNIGHT_DIRS, 8)
        || attackedByStepper(b, r, c, byColor, KING,   KING_DIRS,   8)
        || attackedBySlider (b, r, c, byColor, ROOK,   ROOK_DIRS,   4)
        || attackedBySlider (b, r, c, byColor, BISHOP, BISHOP_DIRS, 4)
        || attackedByPawn   (b, r, c, byColor);
}