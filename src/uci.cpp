#include<iostream>
#include "movegen.h"
#include "perft.h"
#include <sstream>

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
            std::vector<Move> legals= generateLegalMoves(b);
            if(legals.empty()) std::cout<<"bestmove 0000\n"<<std::endl;
            else std::cout << "bestmove "<< move2string(legals[0])<<"\n"<< std::endl;
        }
        else if (command == "quit" || command == "exit")
        {
            break;
        }
        else if (command == "perft")
        {
            std::string dep, fen;
            inp>>dep;
            int depth = std::stoi(dep);
            inp>>fen;
            Board b;
            b.loadFEN(fen);
            long nc=divide(b, depth, true);
            std::cout<<"total nodes: "<<nc<<std::endl;
        }
        else if (command == "ucinewgame")
        {
            b.clear();
        }
    }

    return;
}