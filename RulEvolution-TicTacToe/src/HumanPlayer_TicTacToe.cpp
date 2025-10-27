#include "HumanPlayer_TicTacToe.h"
#include <iostream>
#include <limits>

/**
 * @fn int HumanPlayer::chooseMove(const Board& board)
 * @brief Ask the human user to input a valid move.
 */
int HumanPlayer::chooseMove(const Board& board) {
    int idx;
    while (true) {
        std::cout << "Player " << symbol << ", enter your move (0-8): ";
        if (!(std::cin >> idx)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Try again.\n";
            continue;
        }
        if (board.isEmpty(idx)) {
            return idx;
        }
        else {
            std::cout << "Cell not empty or invalid index. Try again.\n";
        }
    }
}