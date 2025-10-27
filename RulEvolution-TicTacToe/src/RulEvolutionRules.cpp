// ================================================================
//  RulEvolutionRules.cpp  —  Parallel Version (OpenMP Optional)
// ================================================================
#include "RulEvolutionRules.h"
#include <iostream>

#ifdef USE_OMP
#include <omp.h>
#endif

bool RulEvolutionRules::isWinningMove(const Board& board, int idx, char symbol) {
    Board temp = board;
    temp.place(idx, symbol);
    return (temp.winner() == symbol);
}

std::vector<RuleEvaluation> RulEvolutionRules::evaluate(
    const Board& board, char playerSymbol, const std::vector<double>& weights
) {
    std::vector<RuleEvaluation> results;
    char opponent = (playerSymbol == 'X') ? 'O' : 'X';

    const int expectedRules = 5;
    if ((int)weights.size() < expectedRules) {
        std::cout << "[ERROR] evaluate(): weights vector too small! size="
            << weights.size() << ", expected at least " << expectedRules << "\n";
        return results;
    }

#ifdef USE_OMP
#pragma omp parallel
    {
        std::vector<RuleEvaluation> localResults; // thread-local buffer

#pragma omp for nowait
        for (int i = 0; i < 9; i++) {
            if (!board.isEmpty(i)) continue;

            // --- BLOCKING MOVE ---
            if (isWinningMove(board, i, opponent))
                localResults.push_back({ i, RULE_BLOCK, weights[(int)RULE_BLOCK - 1] });

            // --- CENTER ---
            if (i == 4)
                localResults.push_back({ i, RULE_CENTER, weights[(int)RULE_CENTER - 1] });

            // --- CORNER ---
            if (i == 0 || i == 2 || i == 6 || i == 8)
                localResults.push_back({ i, RULE_CORNER, weights[(int)RULE_CORNER - 1] });

            // --- SIDE ---
            if (i == 1 || i == 3 || i == 5 || i == 7)
                localResults.push_back({ i, RULE_SIDE, weights[(int)RULE_SIDE - 1] });

            // --- PREPARATION RULE ---
            int potential = 0;
            int row = i / 3, col = i % 3;

            // Row analysis
            int rowSelf = 0, rowEmpty = 0;
            for (int k = 0; k < 3; ++k) {
                int idx = row * 3 + k;
                if (board.isEmpty(idx)) rowEmpty++;
                else if (board.winner() != playerSymbol) rowSelf++;
            }
            if (rowSelf == 1 && rowEmpty >= 2) potential++;

            // Column analysis
            int colSelf = 0, colEmpty = 0;
            for (int k = 0; k < 3; ++k) {
                int idx = col + 3 * k;
                if (board.isEmpty(idx)) colEmpty++;
                else if (board.winner() != playerSymbol) colSelf++;
            }
            if (colSelf == 1 && colEmpty >= 2) potential++;

            // Diagonal analysis (only for even indices)
            if (i % 2 == 0) {
                int d1Self = 0, d1Empty = 0;
                for (int k : {0, 4, 8}) {
                    if (board.isEmpty(k)) d1Empty++;
                    else if (board.winner() != playerSymbol) d1Self++;
                }
                if (d1Self == 1 && d1Empty >= 2) potential++;

                int d2Self = 0, d2Empty = 0;
                for (int k : {2, 4, 6}) {
                    if (board.isEmpty(k)) d2Empty++;
                    else if (board.winner() != playerSymbol) d2Self++;
                }
                if (d2Self == 1 && d2Empty >= 2) potential++;
            }

            if (potential > 0) {
                double score = weights[(int)RULE_PREPARATION - 1] * potential;
                localResults.push_back({ i, RULE_PREPARATION, score });
            }
        }

        // Thread-safe merge
#pragma omp critical
        {
            results.insert(results.end(), localResults.begin(), localResults.end());
        }
    }
#else
    // --- Sequential fallback (identical logic) ---
    for (int i = 0; i < 9; i++) {
        if (!board.isEmpty(i)) continue;

        if (isWinningMove(board, i, opponent))
            results.push_back({ i, RULE_BLOCK, weights[(int)RULE_BLOCK - 1] });

        if (i == 4)
            results.push_back({ i, RULE_CENTER, weights[(int)RULE_CENTER - 1] });

        if (i == 0 || i == 2 || i == 6 || i == 8)
            results.push_back({ i, RULE_CORNER, weights[(int)RULE_CORNER - 1] });

        if (i == 1 || i == 3 || i == 5 || i == 7)
            results.push_back({ i, RULE_SIDE, weights[(int)RULE_SIDE - 1] });

        int potential = 0;
        int row = i / 3, col = i % 3;

        int rowSelf = 0, rowEmpty = 0;
        for (int k = 0; k < 3; ++k) {
            int idx = row * 3 + k;
            if (board.isEmpty(idx)) rowEmpty++;
            else if (board.winner() != playerSymbol) rowSelf++;
        }
        if (rowSelf == 1 && rowEmpty >= 2) potential++;

        int colSelf = 0, colEmpty = 0;
        for (int k = 0; k < 3; ++k) {
            int idx = col + 3 * k;
            if (board.isEmpty(idx)) colEmpty++;
            else if (board.winner() != playerSymbol) colSelf++;
        }
        if (colSelf == 1 && colEmpty >= 2) potential++;

        if (i % 2 == 0) {
            int d1Self = 0, d1Empty = 0;
            for (int k : {0, 4, 8}) {
                if (board.isEmpty(k)) d1Empty++;
                else if (board.winner() != playerSymbol) d1Self++;
            }
            if (d1Self == 1 && d1Empty >= 2) potential++;

            int d2Self = 0, d2Empty = 0;
            for (int k : {2, 4, 6}) {
                if (board.isEmpty(k)) d2Empty++;
                else if (board.winner() != playerSymbol) d2Self++;
            }
            if (d2Self == 1 && d2Empty >= 2) potential++;
        }

        if (potential > 0) {
            double score = weights[(int)RULE_PREPARATION - 1] * potential;
            results.push_back({ i, RULE_PREPARATION, score });
        }
    }
#endif

    return results;
}