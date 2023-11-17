#include <algorithm>
#include <random>
#include <iostream>
#include <thread>

#include "board.hpp"
#include "engine.hpp"
#include "butils.hpp"

double evaluate(Board b, PlayerColor clr) {
    U8 *pieces = (U8*)(&(b.data));
    double white_score = 0;
    double black_score = 0;
    if (b.in_check()) {
        if (b.data.player_to_play == WHITE) white_score -= 4.0;
        else black_score -= 4.0;
        auto moveset = b.get_legal_moves();
        if (moveset.size() == 0) {
            if (b.data.player_to_play == WHITE) white_score -= 100.0;
            else black_score -= 100.0;
            
        }
    }
    // need to write loops to evaluate score based on board structure
    // evaluations for white and black

    // for (int i = 0; i < b.data.n_pieces; i++) {
    //     U8 piece = pieces[i];
    //     if (piece == DEAD) continue;
    //     bool threat = b.under_threat(piece);
    //     U8 piece_type = b.data.board_0[piece];
    //     if (piece_type & PAWN) {
    //         white_score += 2.0;
    //         if (threat) white_score -= 2.0;
    //     } else if (piece_type & ROOK) {
    //         white_score += 10.5;
    //         if (threat) white_score -= 13.0;
    //     } else if (piece_type & KING) white_score += 0.1;
    //     else if (piece_type & BISHOP) {
    //         white_score += 7.0;
    //         if (threat) white_score -= 8.0;
    //     } 
    //     else if (piece_type & KNIGHT) {
    //         white_score += 7.0;
    //         if (threat) white_score -= 8.0;
    //     }
    // }
    // int s = 10;
    // for (int i = 0; i < b.data.n_pieces; i++) {
    //     U8 piece = pieces[i+s];
    //     if (piece == DEAD) continue;
    //     bool threat = b.under_threat(piece);
    //     U8 piece_type = b.data.board_0[piece];
    //     if (piece_type & PAWN) {
    //         black_score += 2.0;
    //         if (threat) black_score -= 2.0;
    //     } else if (piece_type & ROOK) {
    //         black_score += 10.5;
    //         if (threat) black_score -= 13.0;
    //     } else if (piece_type & KING) black_score += 0.1;
    //     else if (piece_type & BISHOP) {
    //         black_score += 7.0;
    //         if (threat) black_score -= 8.0;
    //     } 
    //     else if (piece_type & KNIGHT) {
    //         black_score += 7.0;
    //         if (threat) black_score -= 8.0;
    //     }
    // }

    for (int i = 0; i < b.data.n_pieces; i++) {
        U8 piece = pieces[i];
        if (piece == DEAD) continue;
        U8 piece_type = b.data.board_0[piece];
        if (piece_type & PAWN) white_score += 2.0;
        else if (piece_type & ROOK) white_score += 10.5;
        else if (piece_type & KING) white_score += 0.1;
        else if (piece_type & BISHOP) white_score += 7.0;
        else if (piece_type & KNIGHT) white_score += 7.0;
    }
    int s = 10;
    for (int i = 0; i < b.data.n_pieces; i++) {
        U8 piece = pieces[i+s];
        if (piece == DEAD) continue;
        U8 piece_type = b.data.board_0[piece];
        if (piece_type & PAWN) black_score += 2.0;
        else if (piece_type & ROOK) black_score += 10.5;
        else if (piece_type & KING) black_score += 0.1;
        else if (piece_type & BISHOP) black_score += 7.0;
        else if (piece_type & KNIGHT) black_score += 7.0;
    }

    // can add piece adjustment values
    if (clr == WHITE) return (white_score - black_score);
    else return (black_score - white_score);
}

// can add endgame tablebases using some techniques
// https://www.chessprogramming.org/Endgame_Tablebases

// minimax with alpha beta pruning
double alpha_beta(Board b, int depth, bool maximise, double alpha, double beta, PlayerColor clr) {
    auto moveset = b.get_legal_moves();
    if ((depth == 3) or (moveset.size() == 0)) {
        return evaluate(b, clr);
    }
    if (maximise) {
        double max_eval = -10000.0;
        for (auto m : moveset) {
            Board c(b);
            c.do_move_(m);
            double eval = alpha_beta(c, depth+1, false, alpha, beta, clr);
            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, max_eval);
            if (beta <= alpha) break;
        }
        return max_eval;
    } else {
        double min_eval = 10000.0;
        for (auto m : moveset) {
            Board c(b);
            c.do_move_(m);
            double eval = alpha_beta(c, depth+1, true, alpha, beta, clr);
            min_eval = std::min(min_eval, eval);
            beta = std::min(beta, min_eval);
            if (beta <= alpha) break;
        }
        return min_eval;
    }
}

double evaluate_2(Board b, PlayerColor clr, U8 move) {
    U8 *pieces = (U8*)(&(b.data));
    double white_score = 0;
    double black_score = 0;
    if (b.in_check()) {
        if (b.data.player_to_play == WHITE) white_score -= 20.0;
        else black_score -= 20.0;
        auto moveset = b.get_legal_moves();
        if (moveset.size() == 0) {
            if (b.data.player_to_play == WHITE) white_score -= 100.0;
            else black_score -= 100.0;
        }
    }

    U8 piece_pos = getp1(move);
    bool threat = b.under_threat(piece_pos);
    U8 piece_type = b.data.board_0[piece_pos];
    if (piece_type & PAWN) {
        if (piece_type & WHITE) white_score += 1.0;
        else black_score += 1.0;
        if (threat) {
            if (piece_type & WHITE) white_score -= 1.0;
            else black_score -= 1.0;
        }
    } else if (piece_type & KING) {
        if (piece_type & WHITE) white_score += 2.0;
        else black_score += 2.0;
    } else if (piece_type & BISHOP) {
        if (piece_type & WHITE) white_score += 3.0;
        else black_score += 3.0;
        if (threat) {
            if (piece_type & WHITE) white_score -= 6.0;
            else black_score -= 6.0;
        }   
    } else if (piece_type & ROOK) {
        if (piece_type & WHITE) white_score += 3.0;
        else black_score += 3.0;
        if (threat) {
            if (piece_type & WHITE) white_score -= 6.0;
            else black_score -= 6.0;
        }
    } else if (piece_type & KNIGHT) {
        if (piece_type & WHITE) white_score += 5.0;
        else black_score += 5.0;
        if (threat) {
            if (piece_type & WHITE) white_score -= 6.0;
            else black_score -= 6.0;
        }
    }

    // evaluation for mobility
    auto moveset = b.get_legal_moves();
    if (b.data.player_to_play == WHITE) white_score += moveset.size() * 0.1;
    else black_score += moveset.size() * 0.1;
    

    for (int i = 0; i < b.data.n_pieces; i++) {
        U8 piece = pieces[i];
        if (piece == DEAD) continue;
        U8 piece_type = b.data.board_0[piece];
        if (piece_type & PAWN) white_score += 2.0;
        else if (piece_type & ROOK) white_score += 10.5;
        else if (piece_type & KING) white_score += 0.1;
        else if (piece_type & BISHOP) white_score += 7.0;
        else if (piece_type & KNIGHT) white_score += 7.0;
    }
    int s = 10;
    for (int i = 0; i < b.data.n_pieces; i++) {
        U8 piece = pieces[i+s];
        if (piece == DEAD) continue;
        U8 piece_type = b.data.board_0[piece];
        if (piece_type & PAWN) black_score += 2.0;
        else if (piece_type & ROOK) black_score += 10.5;
        else if (piece_type & KING) black_score += 0.1;
        else if (piece_type & BISHOP) black_score += 7.0;
        else if (piece_type & KNIGHT) black_score += 7.0;
    }
    // can add piece adjustment values
    if (clr == WHITE) return (white_score - black_score);
    else return (black_score - white_score);
}


// minimax with alpha beta pruning 2nd version

double alpha_beta2(Board b, int depth, bool maximise, double alpha, double beta, PlayerColor clr, U8 move) {
    auto moveset = b.get_legal_moves();
    if ((depth == 2) or (moveset.size() == 0) or (b.in_check())) {
        return evaluate_2(b, clr, move);
    }
    if (maximise) {
        double max_eval = -10000.0;
        for (auto m : moveset) {
            Board c(b);
            c.do_move_(m);
            double eval = alpha_beta2(c, depth+1, !maximise, alpha, beta, clr, m);
            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, max_eval);
            if (beta <= alpha) break;
        }
        return max_eval;
    } else {
        double min_eval = 10000.0;
        for (auto m : moveset) {
            Board c(b);
            c.do_move_(m);
            double eval = alpha_beta2(c, depth+1, !maximise, alpha, beta, clr, m);
            min_eval = std::min(min_eval, eval);
            beta = std::min(beta, min_eval);
            if (beta <= alpha) break;
        }
        return min_eval;
    }
}

void Engine::find_best_move(const Board& b) {

    // pick a random move
    PlayerColor clr = b.data.player_to_play;
    bool search = true;
    
    auto moveset = b.get_legal_moves();
    if (moveset.size() == 0) {
        std::cout << "Could not get any moves from board!\n";
        std::cout << board_to_str(&b.data);
        this->best_move = 0;
    }
    else {
        double best_score = -10000.0;
        double alpha = -10000.0;
        double beta = 10000.0;
        std::unordered_set<U16> best_moves;
        time_t start = time(0);
        for (auto m : moveset) {
            Board newboard(b);
            newboard.do_move_(m);
            if (newboard.get_legal_moves().size() == 0) {
                this->best_move = m;
                search = false;
                break;
            }
            double score = alpha_beta(newboard, 0, false, alpha, beta, clr);
            if (score >= best_score) {
                best_score = score;
                this->best_move = m;
                best_moves.insert(m);
            }
            time_t end = time(0);
            if (end - start >= 1) break;
        }
        if (best_moves.size() >= 5) {
            double best_score2 = -10000.0;
            double alpha2 = -10000.0;
            double beta2 = 10000.0;
            time_t start2 = time(0);
            for (auto m : best_moves) {
                if (!search) break;
                Board newboard(b);
                newboard.do_move_(m);
                double score = alpha_beta2(newboard, 0, false, alpha2, beta2, clr, m);
                if (score > best_score2) {
                    best_score2 = score;
                    this->best_move = m;
                }
                time_t end2 = time(0);
                if (end2 - start2 >= 0.5) break;
            }
        }
    }

    // just for debugging, to slow down the moves
    // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}