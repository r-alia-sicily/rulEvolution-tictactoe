#ifndef PLAYER_TICTACTOE_H
#define PLAYER_TICTACTOE_H

#include "Board_TicTacToe.h"

/**
 * @class Player
 * @brief Abstract base class for Tic-Tac-Toe players.
 */
class Player {
protected:
    char symbol; ///< 'X' or 'O'
public:
    explicit Player(char s) : symbol(s) {}
    virtual ~Player() = default;

    /**
     * @brief Decide the next move (0..8).
     */
    virtual int chooseMove(const Board& board) = 0;

    char getSymbol() const { return symbol; }
};

#endif // PLAYER_TICTACTOE_H