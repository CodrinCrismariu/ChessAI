#include <bits/stdc++.h>
#include "headers/nn.h"
#include "headers/board.h"

#define ENGINE_DEPTH 3

const int ERR = 127;
bool rep = 0;

ChessBoard board;
Colour colour = Colour::White;
std::multiset <std::string> fens;

void switchColour() {
	if (colour == Colour::White) {
		colour = Colour::Black;
	}
	else {
		colour = Colour::White;
	}
}

int eval() {
    return nn_evaluate_fen(board.getFen(colour).c_str());
}

int MinMax(int alpha, int depth = ENGINE_DEPTH) {
    if(rep)
        return 0;

    if(depth == 0)
        return eval();
    
    int mx = -INT_MAX;
    auto moves = board.getPossibleMoves(colour);
    std::vector <int> values;
    for(auto move : moves) {
        switchColour();
        MoveData newmove = board.moveTo(std::make_tuple(std::get<0>(move), std::get<1>(move)), std::make_tuple(std::get<2>(move), std::get<3>(move)));
        std::string fen = board.getFen(colour);
        fens.insert(fen);
        if(fens.count(fen) >= 3) {
            rep = 1;
        }
        int beta = mx;
        int value = -MinMax(-beta, depth - 1);
        rep = 0;
        fens.erase(fens.find(fen));
        values.push_back(value);
        mx = std::max(mx, value);
        board.undoMove(newmove);
        switchColour();
        if(mx > alpha)
            return alpha;
    }

    for(int i = 0; i < values.size(); i++)
        if(values[i] == mx)
            return mx;
    
    if(board.isChecked(colour))
        return -(INT_MAX);
    return 0;
}

std::vector < std::tuple<char, char> > getBestMove() {
    int mx = -INT_MAX;
    auto moves = board.getPossibleMoves(colour);
    std::vector <int> values;
    for(auto move : moves) {
        switchColour();
        MoveData newmove = board.moveTo(std::make_tuple(std::get<0>(move), std::get<1>(move)), std::make_tuple(std::get<2>(move), std::get<3>(move)));
        std::string fen = board.getFen(colour);
        fens.insert(fen);
        if(fens.count(fen) >= 3) {
            rep = 1;
        }
        int value = -MinMax(-mx);
        rep = 0;
        fens.erase(fens.find(fen));
        values.push_back(value);
        mx = std::max(mx, value);
        board.undoMove(newmove);
        switchColour();
    }

    for(int i = 0; i < values.size(); i++)
        if(values[i] == mx) {
            return {
                std::make_tuple(std::get<0>(moves[i]), std::get<1>(moves[i])),
                std::make_tuple(std::get<2>(moves[i]), std::get<3>(moves[i]))
            };
        }
    return {};
}

int main() {

    // init engine
    nn_init("NeuralNetworkData");

    std::vector <MoveData> history;
    MoveData move;

    while(true) {
        if(board.getPossibleMoves(colour).empty()) {
            if(board.isChecked(colour)) {
                std::cout << "checkmate ";
                if(colour == Colour::Black)
                    std::cout << "white\n";
                else
                    std::cout << "black\n";
            }
            else
                std::cout << "STALEMATE";
            return 0;
        }
        std::string command;
        std::cin >> command;

        if(command == "getMoves") {
            auto moves = board.getPossibleMoves(colour);
            std::cout << "moves ";
            for(auto moveS : moves)
                std::cout << (char)(std::get<0>(moveS) + 'a') << (char)(std::get<1>(moveS) + '1') << (char)(std::get<2>(moveS) + 'a') << (char)(std::get<3>(moveS) + '1');
            std::cout << '\n';
        } else if(command == "undo") {
            if(history.size()) {
                board.undoMove(history.back());
                switchColour();
                history.pop_back();
                std::cout << "Fen " << board.getFen(colour) << '\n';
            }
        } else if(command == "getFen") {
            std::cout << "Fen " << board.getFen(colour) << '\n';
        } else if(command == "eval") {  
            if(colour == Colour::Black)
                std::cout << "eval " << -eval() << '\n';
            else
                std::cout << "eval " << eval() << '\n';
        } else if(command == "make") {
            auto k = getBestMove();
            move = board.moveTo(k[0], k[1]);
            if (!move.validMove) {
                std::cout << "error ENGINE_MADE_A_WRONG_MOVE" << std::endl;
            }
            else {
                history.push_back(move);
                std::cout << "engine " << (char)(std::get<0>(k[0]) + 'a') << (char)(std::get<1>(k[0]) + '1') << (char)(std::get<0>(k[1]) + 'a') << (char)(std::get<1>(k[1]) + '1') << '\n';
                switchColour();
                fens.insert(board.getFen(colour));
            }
        } else if(command.size() == 4) {
            std::tuple<char, char> orig = std::tuple<char, char>(command[0] - 'a', command[1] - '1');
            std::tuple<char, char> dest = std::tuple<char, char>(command[2] - 'a', command[3] - '1');
            if ((std::get<0>(orig) != ERR) && (std::get<1>(orig) >= 0) && (std::get<1>(orig) <= 7) &&
                    (std::get<0>(dest) != ERR) && (std::get<1>(dest) >= 0) && (std::get<1>(dest) <= 7) &&
                    (board.squares[std::get<0>(orig)][std::get<1>(orig)]!= nullptr) &&
                    (board.squares[std::get<0>(orig)][std::get<1>(orig)]->colour == colour)) {
                move = board.moveTo(orig, dest);
                if (!move.validMove) {
                    std::cout << "error INVALID_MOVE" << std::endl;
                }
                else {
                    history.push_back(move);
                    switchColour();
                    fens.insert(board.getFen(colour));
                }
            }
            else std::cout << "error INVALID_COORDONATES" << std::endl;
        } else {
            std::cout << "error INVALID_COMMAND" << std::endl;
        }
    }

    return 0;
}