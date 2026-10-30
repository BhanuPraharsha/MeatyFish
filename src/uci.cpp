#include<iostream>
#include "movegen.h"
#include "perft.h"
#include "search.h"
#include <sstream>
#include <chrono>
#include <iomanip>

// positions used by the "bench" command
static const char* BENCH_FENS[] = {
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
    "6k1/5ppp/8/8/8/8/8/R3K3 w - - 0 1",
    "4k3/8/8/3q4/8/8/8/3QK3 w - - 0 1"
};

static double millisSince(std::chrono::steady_clock::time_point start)
{
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
}

static void runBench(int depth)
{
    std::cout<<"depth "<<depth<<" (plain negamax is only run up to depth 4)\n";
    long long totalPlain=0, totalPruned=0;
    int mismatches=0;

    for(const char* fen : BENCH_FENS)
    {
        Board pb;
        pb.loadFEN(fen);

        long long prunedNodes=0, plainNodes=0;
        auto t0=std::chrono::steady_clock::now();
        int prunedScore=benchmark(pb, depth, true, prunedNodes);
        double prunedMs=millisSince(t0);
        totalPruned+=prunedNodes;

        std::cout<<std::left<<std::setw(34)<<std::string(fen).substr(0, 33)
                 <<" score "<<std::setw(8)<<prunedScore
                 <<" pruned "<<std::setw(10)<<prunedNodes<<std::fixed<<std::setprecision(1)<<std::setw(8)<<prunedMs<<"ms";

        if(depth<=4)
        {
            t0=std::chrono::steady_clock::now();
            int plainScore=benchmark(pb, depth, false, plainNodes);
            double plainMs=millisSince(t0);
            totalPlain+=plainNodes;
            std::cout<<"  plain "<<std::setw(10)<<plainNodes<<std::setw(8)<<plainMs<<"ms";
            if(plainScore!=prunedScore)
            {
                mismatches++;
                std::cout<<"  SCORE MISMATCH (plain "<<plainScore<<")";
            }
        }
        std::cout<<"\n";
    }

    std::cout<<"total pruned nodes: "<<totalPruned;
    if(depth<=4) std::cout<<", total plain nodes: "<<totalPlain<<", score mismatches: "<<mismatches;
    std::cout<<std::endl;
}

void uciloop()
{
    Board b;
    std::string line;

    while(std::getline(std::cin, line))
    {
        std::string command;
        if(line.empty()) continue;
        std::istringstream inp(line);
        inp>>command;
        if(command=="uci")
        {
            std::cout << "id name MeatyFish\n";
            std::cout << "id author BhanuPraharsha\n";
            std::cout << "uciok\n" << std::endl;
        }
        else if (command == "isready")
        {
            std::cout << "readyok\n"<< std::endl;
        }
        else if (command == "position")
        {
            std::string type, word, fen;
            inp >> type;

            if (type == "startpos")
            {
                fen=START_POS;
                inp>>word; //moves
            }
            else if (type == "fen")
            {
                while(inp>>word && word != "moves") fen += word + " ";
            }

            b.loadFEN(fen);

            if(word=="moves")
            {
                while(inp>>word)
                {
                    Move m = string2move(word, b);
                    b.makeMove(m);
                }
            }
        }
        else if (command == "go")
        {
            long long nodesVis=0;
            int depth=6;
            Move BestMove=bestMove(b, depth, nodesVis);
            std::cout<<"info depth "<<depth<<" nodes "<<nodesVis<<" \n"<<std::endl;
            if(BestMove.from==0 && BestMove.to==0) std::cout<<"bestmove 0000\n"<<std::endl;
            else std::cout << "bestmove "<< move2string(BestMove)<<"\n"<< std::endl;
        }
        else if (command == "quit" || command == "exit")
        {
            break;
        }
        else if (command == "perft")
        {
            std::string dep, fen, word;
            inp>>dep;
            int depth = std::stoi(dep);
            while(inp>>word) fen += word + " ";
            Board pb;
            pb.loadFEN(fen);
            long nc=divide(pb, depth, true);
            std::cout<<"total nodes: "<<nc<<std::endl;
        }
        else if (command == "bench")
        {
            int depth=4;
            inp>>depth;
            runBench(depth);
        }
        else if (command == "ucinewgame")
        {
            b.clear();
        }
    }

    return;
}