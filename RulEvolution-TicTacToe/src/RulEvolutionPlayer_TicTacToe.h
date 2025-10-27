#ifndef RULEVOLUTIONPLAYER_TICTACTOE_H
#define RULEVOLUTIONPLAYER_TICTACTOE_H

#include "Player_TicTacToe.h"
#include "LearningState.h"
#include "RulEvolutionRules.h"   // for RuleType
#include <vector>

/**
 * @class RulEvolutionPlayer
 * @brief Rule-based player that uses a LearningState to evolve decision-making.
 */
class RulEvolutionPlayer : public Player {
public:
    /**
     * @brief Construct a RulEvolutionPlayer.
     * @param s Symbol ('X' or 'O')
     * @param initState Initial learning state (default: empty)
     */
    explicit RulEvolutionPlayer(char s, const LearningState& initState = LearningState());

    /**
     * @brief Choose a move (standard Player interface)
     * @param board Current game board
     * @return Index (0–8) of the chosen move
     */
    int chooseMove(const Board& board) override;

    /**
     * @brief Choose a move while also reporting which rules were used
     * @param board Current board state
     * @param rulesUsed Vector filled with the rules that influenced the decision
     * @return Index (0–8) of the chosen move
     */
    int chooseMove(const Board& board, std::vector<RuleType>& rulesUsed);

    /**
     * @brief Accessor for the internal LearningState (read-only)
     */
    const LearningState& getState() const { return state; }

    /**
     * @brief Mutator for the internal LearningState
     */
    void setState(const LearningState& newState) { state = newState; }

private:
    LearningState state;  ///< Current learning weights and parameters
};

#endif