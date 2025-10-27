#ifndef HUMANPLAYER_TICTACTOE_H
#define HUMANPLAYER_TICTACTOE_H

#include "Player_TicTacToe.h"

class HumanPlayer : public Player {
public:
    explicit HumanPlayer(char s) : Player(s) {}
    int chooseMove(const Board& board) override;
};

#endif // HUMANPLAYER_TICTACTOE_H