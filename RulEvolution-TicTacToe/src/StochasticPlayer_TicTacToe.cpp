#include "StochasticPlayer_TicTacToe.h"
#include <cstdlib>
#include <vector>
#include <iostream>

/**
 * @fn int StochasticPlayer::chooseMove(const Board& board)
 * @brief Choose a random valid move from available cells.
 */
int StochasticPlayer::chooseMove(const Board& board) {
    std::vector<int> available;
    for (int i = 0; i < 9; i++) {
        if (board.isEmpty(i)) available.push_back(i);
    }
    if (available.empty()) return 0;

    int r = std::rand() % available.size();
    int move = available[r];

    std::cout << "[StochasticPlayer] chose move " << move << "\n";

    return move;
}