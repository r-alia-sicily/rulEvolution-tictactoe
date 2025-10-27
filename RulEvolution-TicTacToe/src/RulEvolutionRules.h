#ifndef RULEVOLUTIONRULES_H
#define RULEVOLUTIONRULES_H

#include "Board_TicTacToe.h"
#include "RuleType.h"   // Include the enum and ruleToString definition
#include <vector>
#include <string>

/**
 * @struct RuleEvaluation
 * @brief Result of evaluating a RulEvolution rule on a given board state.
 */
struct RuleEvaluation {
    int moveIndex;       // Index of the suggested cell
    RuleType ruleIndex;  // Which rule generated this suggestion
    double score;        // Weighted score from that rule
};

/**
 * @class RulEvolutionRules
 * @brief Collection of RulEvolution rules for Tic-Tac-Toe.
 */
class RulEvolutionRules {
public:
    static std::vector<RuleEvaluation> evaluate(
        const Board& board,
        char playerSymbol,
        const std::vector<double>& weights
    );

private:
    static bool isWinningMove(const Board& board, int idx, char symbol);
};

#endif // RULEVOLUTIONRULES_H