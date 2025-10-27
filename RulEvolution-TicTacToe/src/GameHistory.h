#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H

#include <vector>
#include "RulEvolutionRules.h"  // for RuleType

/**
 * @struct MoveRecord
 * @brief Represents a single move with all rules that suggested it.
 */
struct MoveRecord {
    int move;                        ///< Cell index chosen
    std::vector<RuleType> rules;     ///< List of rules that supported this move
};

/**
 * @class GameHistory
 * @brief Stores the sequence of moves and their associated RulEvolution rules.
 * It also keeps track of the match outcome for learning purposes.
 */
class GameHistory {
public:
    std::vector<MoveRecord> moves;   ///< Sequence of all moves made in the match
    bool hasWon;                     ///< True if the RulEvolution player has won
    int totalMoves;                  ///< Total number of moves recorded
    int rulevMoves;                  ///< Moves made by the RulEvolution player only

    /**
     * @brief Default constructor. Initializes all counters.
     */
    GameHistory()
        : hasWon(false), totalMoves(0), rulevMoves(0) {
    }

    /**
     * @brief Add a move with the rules that supported it.
     * @param move The chosen cell index
     * @param rules The list of rules applied for this move
     */
    void addMove(int move, const std::vector<RuleType>& rules) {
        moves.push_back({ move, rules });
        totalMoves++;
        if (!rules.empty())
            rulevMoves++;
    }

    /**
     * @brief Clear the game history.
     */
    void clear() noexcept {
        moves.clear();
        hasWon = false;
        totalMoves = 0;
        rulevMoves = 0;
    }

    /**
     * @brief Set the match outcome for the RulEvolution player.
     * @param win True if the player has won the match.
     */
    void setResult(bool win) noexcept {
        hasWon = win;
    }

    /**
     * @brief Returns the number of recorded moves.
     * @return int Number of moves stored.
     */
    int size() const noexcept {
        return static_cast<int>(moves.size());
    }
};

#endif // GAMEHISTORY_H