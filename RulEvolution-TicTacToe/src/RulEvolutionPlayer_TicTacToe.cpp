#include "RulEvolutionPlayer_TicTacToe.h"
#include "RulEvolutionRules.h"
#include <map>
#include <iostream>
#include <algorithm>
#include <cstdlib>

RulEvolutionPlayer::RulEvolutionPlayer(char s, const LearningState& initState)
    : Player(s), state(initState) {
    if (state.weights.empty()) {
        state.weights.resize(5, 0.5); // only 5 adaptive rules
        std::cout << "[INIT] RulEvolutionPlayer: weights initialized (5 adaptive rules)\n";
    }
}

int RulEvolutionPlayer::chooseMove(const Board& board) {
    std::vector<RuleType> dummy;
    return chooseMove(board, dummy);
}

int RulEvolutionPlayer::chooseMove(const Board& board, std::vector<RuleType>& rulesUsed) {
    rulesUsed.clear();

    // 1️⃣ Absolute WIN rule — always checked first
    for (int i = 0; i < 9; ++i) {
        if (board.isEmpty(i)) {
            Board temp = board;
            temp.place(i, symbol);
            if (temp.winner() == symbol) {
                rulesUsed.push_back(RULE_WIN);
                std::cout << "[RulEvolutionPlayer] Absolute WIN rule applied at cell " << i << "\n";
                return i;
            }
        }
    }

    // 2️⃣ Evaluate remaining adaptive rules
    auto evals = RulEvolutionRules::evaluate(board, symbol, state.weights);
    if (evals.empty()) {
        for (int i = 0; i < 9; ++i)
            if (board.isEmpty(i))
                return i;
        return 0;
    }

    std::map<int, double> scoreMap;
    std::map<int, std::vector<RuleType>> ruleMap;

    for (const auto& e : evals) {
        if (e.moveIndex < 0 || e.moveIndex >= 9) continue;
        scoreMap[e.moveIndex] += e.score;
        ruleMap[e.moveIndex].push_back(e.ruleIndex);
    }

    // 3️⃣ Reflective-Exploration probabilistic choice
    double totalScore = 0.0;
    for (const auto& kv : scoreMap)
        if (kv.second > 0) totalScore += kv.second;

    double r = ((double)std::rand() / RAND_MAX) * totalScore;
    double cumulative = 0.0;
    int chosenMove = -1;

    for (const auto& kv : scoreMap) {
        cumulative += kv.second;
        if (r <= cumulative) {
            chosenMove = kv.first;
            break;
        }
    }

    if (chosenMove < 0) {
        for (int i = 0; i < 9; ++i)
            if (board.isEmpty(i)) { chosenMove = i; break; }
    }

    auto it = ruleMap.find(chosenMove);
    if (it != ruleMap.end())
        rulesUsed = it->second;

    std::cout << "[RulEvolutionPlayer] chose cell " << chosenMove
        << " (probabilistic selection)\n";
    return chosenMove;
}