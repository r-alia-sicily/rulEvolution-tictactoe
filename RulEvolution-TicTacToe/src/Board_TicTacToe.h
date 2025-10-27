#ifndef BOARD_TICTACTOE_H
#define BOARD_TICTACTOE_H

#include <array>
#include <iostream>

/**
 * @class Board
 * @brief Represents a Tic-Tac-Toe board with 9 cells.
 *
 * Each cell can hold 'X', 'O', or ' ' (empty).
 * Provides basic operations for game management:
 * resetting, placing moves, checking status, and printing.
 */
class Board {
private:
    std::array<char, 9> cells; ///< Board cells indexed from 0 to 8

public:
    /**
     * @fn Board::Board()
     * @brief Default constructor. Initializes the board.
     */
    Board();

    /**
     * @fn void Board::reset()
     * @brief Reset the board to empty state.
     */
    void reset();

    /**
     * @fn bool Board::place(int idx, char player)
     * @brief Place a symbol ('X' or 'O') at a given position.
     * @param idx Index of the cell (0..8).
     * @param player Symbol to place ('X' or 'O').
     * @return true if placement is valid and successful,
     * false if index is invalid or cell already occupied.
     */
    bool place(int idx, char player);

    /**
     * @fn bool Board::isEmpty(int idx) const
     * @brief Check if a given cell is empty.
     * @param idx Index of the cell (0..8).
     * @return true if empty, false otherwise.
     */
    bool isEmpty(int idx) const;

    /**
     * @fn bool Board::isFull() const
     * @brief Check if the board is completely filled.
     * @return true if all cells are occupied, false otherwise.
     */
    bool isFull() const;

    /**
     * @fn char Board::winner() const
     * @brief Determine if there is a winner.
     * @return 'X' or 'O' if a winner exists, ' ' if no winner.
     */
    char winner() const;

    /**
     * @fn void Board::print() const
     * @brief Print the current board to the console in 3x3 format.
     */
    void print() const;
};

#endif // BOARD_TICTACTOE_H