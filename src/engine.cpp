#include <algorithm>
#include <random>
#include <iostream>
#include <thread>

#include "board.hpp"
#include "engine.hpp"
#include "butils.hpp"

double evaluate(Board* b, PlayerColor clr) {
    U8 *pieces = (U8*)(&(b->data));
    double white_score = 0;
    double black_score = 0;
    if (b->in_check()) {
        if (b->data.player_to_play == WHITE) white_score -= 4.0;
        else black_score -= 4.0;
        auto moveset = b->get_legal_moves();
        if (moveset.size() == 0) {
            if (b->data.player_to_play == WHITE) white_score -= 100.0;
            else black_score -= 100.0;
        }
    }
    // need to write loops to evaluate score based on board structure
    // evaluations for white and black
    for (int i = 0; i < b->data.n_pieces; i++) {
        U8 piece = pieces[i];
        if (piece == DEAD) continue;
        U8 piece_type = b->data.board_0[piece];
        if (piece_type & PAWN) white_score += 1.0;
        else if (piece_type & ROOK) white_score += 14.0;
        else if (piece_type & KING) white_score += 40.0;
        else if (piece_type & BISHOP) white_score += 7.0;
        else if (piece_type & KNIGHT) white_score += 5.0;
    }
    int s = 10;
    for (int i = 0; i < b->data.n_pieces; i++) {
        U8 piece = pieces[i+s];
        if (piece == DEAD) continue;
        U8 piece_type = b->data.board_0[piece];
        if (piece_type & PAWN) black_score += 1.0;
        else if (piece_type & ROOK) black_score += 14.0;
        else if (piece_type & KING) black_score += 40.0;
        else if (piece_type & BISHOP) black_score += 7.0;
        else if (piece_type & KNIGHT) black_score += 5.0;
    }
    if (clr == WHITE) return (white_score - black_score);
    else return (black_score - white_score);
}

void Engine::find_best_move(const Board& b) {

    // pick a random move
    PlayerColor clr = b.data.player_to_play;
    
    auto moveset = b.get_legal_moves();
    if (moveset.size() == 0) {
        std::cout << "Could not get any moves from board!\n";
        std::cout << board_to_str(&b.data);
        this->best_move = 0;
    }
    else {
        std::vector<U16> moves;
        std::cout << show_moves(&b.data, moveset) << std::endl;
        for (auto m : moveset) {
            std::cout << move_to_str(m) << " ";
        }
        std::cout << std::endl;
        std::sample(
            moveset.begin(),
            moveset.end(),
            std::back_inserter(moves),
            1,
            std::mt19937{std::random_device{}()}
        );
        this->best_move = moves[0];
    }

    // just for debugging, to slow down the moves
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}
