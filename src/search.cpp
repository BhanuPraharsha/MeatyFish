#include<iostream>
#include<vector>
#include "board.h"
#include "movegen.h"
#include "eval.h"
#include<algorithm>

static int scoreMove(const Board &b, const Move &m) //heuristics for move ordering
{
    int score=0;
    if(m.enPassant) //enpassant capture
    {
        score += 9*PIECE_VALUE[PAWN];
    }
    else if((b.pieceAt(m.to) & TYPE_MASK)!=NONE)  // captures
    {
        score +=(10*PIECE_VALUE[(b.pieceAt(m.to)&TYPE_MASK)]) - PIECE_VALUE[(b.pieceAt(m.from)&TYPE_MASK)];
    }
    if(m.promotion)
    {
        score += PIECE_VALUE[m.promotion] * 1000;
    }

    return score;
}


static bool isNoisy(const Move &m, const Board &b)
{
    return (((b.pieceAt(m.to) & TYPE_MASK) != NONE) || m.enPassant  || (m.promotion == QUEEN));
}



static int quiescenceSearch(int alpha, int beta, Board &b, long long &nodesVis)
{
    int baseVal=evaluate(b); //baseline
    if(baseVal >= beta)
    {
        return beta;
    }
    alpha=std::max(alpha, baseVal);

    std::vector<Move> moves=generateLegalMoves(b);
    std::vector<Move> noisyMoves;
    for(const auto& move: moves)
    {
        if(isNoisy(move, b)) noisyMoves.push_back(move);
    }
    std::sort(noisyMoves.begin(), noisyMoves.end(), [&](const Move& a, const Move& c){return scoreMove(b, a) > scoreMove(b, c);});

    for(const auto& move: noisyMoves)
    {
        b.makeMove(move);
        nodesVis++;
        int curr=-quiescenceSearch(-beta, -alpha, b, nodesVis);
        b.unmakeMove();

        if(curr >= beta) return beta;
        alpha=std::max(alpha, curr);
    }

    return alpha;
}

static int negamax(Board &b, int depth, int distFromRoot, long long &nodesVis)
{
    if(depth==0) return evaluate(b);
    std::vector<Move> moves=generateLegalMoves(b);

    if(moves.size()==0)
    {
        if(isSquareAttacked(b, kingSquare(b), b.getSidetoMove()^COLOR_MASK))
        {
            return -999999 + distFromRoot;//checkmate, preventing faster checkmate and forcing faster checkmate
        }
        else return 0; //stalemate
    }

    int best=-9999999;
    for(const auto& move : moves)
    {
        b.makeMove(move);
        nodesVis++;
        best=std::max(best, -negamax(b, depth-1, distFromRoot + 1, nodesVis));
        b.unmakeMove();
    }

    return best;
}

static int alphaBetaPrune(Board &b, int depth, int distFromRoot, int alpha, int beta, long long &nodesVis)
{
    if(depth==0) return quiescenceSearch(alpha, beta, b, nodesVis);
    std::vector<Move> moves=generateLegalMoves(b);
    //move ordering
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& c){return scoreMove(b, a) > scoreMove(b, c);});

    if(moves.size()==0)
    {
        if(isSquareAttacked(b, kingSquare(b), b.getSidetoMove()^COLOR_MASK))
        {
            return -999999 + distFromRoot;//checkmate, preventing faster checkmate and forcing faster checkmate
        }
        else return 0; //stalemate
    }

    for(const auto& move: moves)
    {
        b.makeMove(move);
        nodesVis++;
        int curr=-alphaBetaPrune(b, depth-1, distFromRoot+1, -beta, -alpha, nodesVis);
        b.unmakeMove();
        if(curr >= beta) return beta;
        alpha=std::max(alpha, curr);
    }

    return alpha;
}


Move bestMove(Board &b, int depth, long long &nodesVis)
{
    if(depth==0) return Move();
    std::vector<Move> moves=generateLegalMoves(b);
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& c){return scoreMove(b, a) > scoreMove(b, c);});

    nodesVis=1;

    if(moves.size()==0)
    {
        if(isSquareAttacked(b, kingSquare(b), b.getSidetoMove()^COLOR_MASK))
        {
            return Move({0, 0});
        }
        else return Move({0, 0});
    }

    Move bestMove=moves[0];
    int alpha=-1000000, beta=1000000;

    for(const auto& move : moves)
    {
        b.makeMove(move);
        int currBest=-alphaBetaPrune(b, depth-1, 1, -beta, -alpha, nodesVis);
        b.unmakeMove();
        if(currBest >= beta)
        {
            alpha=beta;
            bestMove=move;
            break;
        }
        if(currBest > alpha)
        {
            alpha=currBest;
            bestMove=move;
        }
    }

    return bestMove;
}


int benchmark(Board &b, int depth, bool prune, long long &nodes)
{
    nodes=0;
    if(prune) return alphaBetaPrune(b, depth, 0, -1000000, 1000000, nodes);
    return negamax(b, depth, 0, nodes);
}
