#include "Board_TicTacToe.h"

/**
 * @fn Board::Board()
 * @brief Construct a new Board and reset it.
 */
Board::Board() {
    reset();
}

/**
 * @fn void Board::reset()
 * @brief Reset all cells to empty (' ').
 */
void Board::reset() {
    cells.fill(' ');
}

/**
 * @fn bool Board::place(int idx, char player)
 * @brief Place a symbol ('X' or 'O') at a given index.
 * @param idx Index of the cell (0..8).
 * @param player Symbol to place ('X' or 'O').
 * @return true if placement is valid and successful,
 * false if index is invalid or cell already occupied.
 */
bool Board::place(int idx, char player) {
    if (idx < 0 || idx >= 9) {
        std::cout << "[ERROR] Invalid index in Board::place(): " << idx << std::endl;
        return false;
    }
    if (cells[idx] == ' ') {
        cells[idx] = player;
        return true;
    }
    return false;
}

/**
 * @fn bool Board::isEmpty(int idx) const
 * @brief Check if a given cell is empty.
 * @param idx Index of the cell (0..8).
 * @return true if empty, false otherwise.
 */
bool Board::isEmpty(int idx) const {
    return (idx >= 0 && idx < 9 && cells[idx] == ' ');
}

/**
 * @fn bool Board::isFull() const
 * @brief Check if the board is completely filled.
 * @return true if all cells are occupied, false otherwise.
 */
bool Board::isFull() const {
    for (char c : cells) {
        if (c == ' ') return false;
    }
    return true;
}

/**
 * @fn char Board::winner() const
 * @brief Determine if there is a winner.
 * @return 'X' or 'O' if a winner exists, ' ' if no winner.
 */
char Board::winner() const {
    // Rows
    for (int r = 0; r < 3; r++) {
        if (cells[3 * r] != ' ' && cells[3 * r] == cells[3 * r + 1] && cells[3 * r] == cells[3 * r + 2])
            return cells[3 * r];
    }
    // Columns
    for (int c = 0; c < 3; c++) {
        if (cells[c] != ' ' && cells[c] == cells[c + 3] && cells[c] == cells[c + 6])
            return cells[c];
    }
    // Diagonals
    if (cells[0] != ' ' && cells[0] == cells[4] && cells[0] == cells[8]) return cells[0];
    if (cells[2] != ' ' && cells[2] == cells[4] && cells[2] == cells[6]) return cells[2];

    return ' '; // No winner
}

/**
 * @fn void Board::print() const
 * @brief Print the board in a human-readable 3x3 format.
 */
void Board::print() const {
    for (int r = 0; r < 3; r++) {
        std::cout << " " << cells[3 * r] << " | " << cells[3 * r + 1] << " | " << cells[3 * r + 2] << "\n";
        if (r < 2) std::cout << "---+---+---\n";
    }
}