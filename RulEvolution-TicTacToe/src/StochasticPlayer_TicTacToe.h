#ifndef STOCHASTICPLAYER_TICTACTOE_H
#define STOCHASTICPLAYER_TICTACTOE_H

#include "Player_TicTacToe.h"

/**
 * @class StochasticPlayer
 * @brief A Tic-Tac-Toe player that chooses moves randomly (stochastically).
 */
class StochasticPlayer : public Player {
public:
    explicit StochasticPlayer(char s) : Player(s) {}
    int chooseMove(const Board& board) override;
};

#endif // STOCHASTICPLAYER_TICTACTOE_H