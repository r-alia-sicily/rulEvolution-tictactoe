// ============================================================================
//  Main_TicTacToe.cpp — Standard Mode + Super-Training Parallel Mode
//  Author: Rosario Alia
//  Description:
//     Standard mode -> interactive play with any player types.
//     Super-Training mode -> automatic parallel training between AIs.
//     Execution time measured for performance comparison.
// ============================================================================

#include "Game_TicTacToe.h"
#include "LearningModule.h"
#include "HumanPlayer_TicTacToe.h"
#include "StochasticPlayer_TicTacToe.h"
#include "RulEvolutionPlayer_TicTacToe.h"
#include "WeightsIO.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <vector>
#include <sstream>
#include <limits>

#ifdef USE_OMP
#include <omp.h>
#endif

LearningModule learner(0.02);

/**
 * @brief Convert a string to lowercase.
 */
auto normalize = [](std::string s) {
    for (size_t i = 0; i < s.size(); ++i)
        s[i] = static_cast<char>(std::tolower(s[i]));
    return s;
    };

/**
 * @brief Program entry point.
 */
int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::cout << "=== RulEvolution TicTacToe ===\n";

#ifdef USE_OMP
    std::cout << "[OpenMP] Parallel mode available. Threads: "
        << omp_get_max_threads() << "\n";
#else
    std::cout << "[OpenMP] Sequential mode (OpenMP disabled).\n";
#endif

    // =====================================================
    // 1. LOAD OR RESET WEIGHTS
    // =====================================================
    std::cout << "Load previously learned weights? (y/n): ";
    char choice;
    std::cin >> choice;
    bool loadPrev = (choice == 'y' || choice == 'Y');

    if (loadPrev) {
        if (WeightsIO::load(learner, "weights_data.txt"))
            std::cout << "[INFO] Loaded weights from file.\n";
        else
            std::cout << "[WARN] No previous weights found. Using defaults.\n";
    }
    else {
        learner.setRuleParameters(RULE_BLOCK, 0.278, 5.0);
        learner.setRuleParameters(RULE_PREPARATION, 0.222, 5.0);
        learner.setRuleParameters(RULE_CENTER, 0.222, 5.0);
        learner.setRuleParameters(RULE_CORNER, 0.167, 5.0);
        learner.setRuleParameters(RULE_SIDE, 0.111, 5.0);
    }

    learner.recordInitialWeights();

    // =====================================================
    // 2. ASK FOR SUPER-TRAINING MODE
    // =====================================================
    std::cout << "\nEnable Super-Training Mode? (y/n): ";
    char superChoice;
    std::cin >> superChoice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    bool superTraining = (superChoice == 'y' || superChoice == 'Y');

    // =====================================================
    // MODE A: SUPER-TRAINING PARALLEL MODE
    // =====================================================
    if (superTraining) {
        std::cout << "\nSelect Super-Training scenario:\n";
        std::cout << "1) Stochastic vs RulEvolution\n";
        std::cout << "2) RulEvolution vs RulEvolution\n";
        std::cout << "Choice (1/2): ";
        int scenario;
        std::cin >> scenario;

        int numMatches;
        std::cout << "Number of training matches: ";
        std::cin >> numMatches;

        Player* pX = nullptr;
        Player* pO = nullptr;

        if (scenario == 1) {
            pX = new StochasticPlayer('X');
            pO = new RulEvolutionPlayer('O');
        }
        else {
            pX = new RulEvolutionPlayer('X');
            pO = new RulEvolutionPlayer('O');
        }

        std::cout << "\n[MODE] Super-Training Parallel Batch Activated.\n";

#ifdef USE_OMP
        double startTime = omp_get_wtime();
#else
        auto startTime = std::clock();
#endif

        std::vector<LearningModule> learners(numMatches, learner);

#ifdef USE_OMP
#pragma omp parallel for
#endif
        for (int i = 0; i < numMatches; ++i) {
            Game g(pX, pO);
            GameHistory history;
            char winner = g.playAndLearn(history, false);
            bool rulevWon = (winner == 'X' || winner == 'O');
            learners[i].updateFromGame(history, rulevWon);

            int tid = 0;
#ifdef USE_OMP
            tid = omp_get_thread_num();
#endif
#pragma omp critical
            {
                std::cout << "[Thread " << tid << "] Match " << (i + 1)
                    << " finished. Winner: "
                    << (winner == ' ' ? "Draw" : std::string(1, winner))
                    << "\n";
            }
        }

        // === MERGE STEP ===
        std::unordered_map<RuleType, double> globalWeights = learner.getWeights();

        for (size_t i = 0; i < learners.size(); ++i) {
            std::unordered_map<RuleType, double> localWeights = learners[i].getWeights();
            for (auto it = localWeights.begin(); it != localWeights.end(); ++it)
                globalWeights[it->first] += it->second;
        }

        for (auto it = globalWeights.begin(); it != globalWeights.end(); ++it) {
            RuleType rule = it->first;
            double averaged = it->second / learners.size();
            learner.setWeight(rule, averaged);
        }

        // === UPDATE TRAINING STATS ===
        for (int i = 0; i < numMatches; ++i) {
            if (scenario == 1)
                learner.incrementTrainingCount("StochasticVsRulev");
            else
                learner.incrementTrainingCount("RulevVsRulev");
        }

#ifdef USE_OMP
        double elapsed = omp_get_wtime() - startTime;
#else
        double elapsed = double(std::clock() - startTime) / CLOCKS_PER_SEC;
#endif
        std::cout << "\n[TIME] Super-Training elapsed: " << elapsed << " s\n";
        std::cout << "[INFO] Super-Training merge complete.\n";

        delete pX;
        delete pO;
    }

    // =====================================================
    // MODE B: STANDARD SEQUENTIAL MODE
    // =====================================================
    else {
        std::string typeX, typeO;
        std::cout << "\nChoose player X type (human/stochastic/rulevolution): ";
        std::cin >> typeX;
        typeX = normalize(typeX);

        std::cout << "Choose player O type (human/stochastic/rulevolution): ";
        std::cin >> typeO;
        typeO = normalize(typeO);

        Player* pX = nullptr;
        Player* pO = nullptr;

        if (typeX == "human" || typeX == "h") pX = new HumanPlayer('X');
        else if (typeX == "stochastic" || typeX == "s") pX = new StochasticPlayer('X');
        else if (typeX == "rulevolution" || typeX == "r") pX = new RulEvolutionPlayer('X');
        else { std::cout << "Invalid choice for X.\n"; return 0; }

        if (typeO == "human" || typeO == "h") pO = new HumanPlayer('O');
        else if (typeO == "stochastic" || typeO == "s") pO = new StochasticPlayer('O');
        else if (typeO == "rulevolution" || typeO == "r") pO = new RulEvolutionPlayer('O');
        else { std::cout << "Invalid choice for O.\n"; delete pX; return 0; }

        int numMatches;
        std::cout << "\nNumber of matches to play: ";
        std::cin >> numMatches;

#ifdef USE_OMP
        double startTime = omp_get_wtime();
#else
        auto startTime = std::clock();
#endif

        for (int i = 1; i <= numMatches; ++i) {
            std::cout << "\n--- Match " << i << " ---\n";
            Game g(pX, pO);
            GameHistory history;
            char winner = g.playAndLearn(history, true);

            if (winner == ' ')
                std::cout << "Result: Draw.\n";
            else
                std::cout << "Winner: " << winner << "\n";

            if (dynamic_cast<RulEvolutionPlayer*>(pX) && dynamic_cast<RulEvolutionPlayer*>(pO))
                learner.incrementTrainingCount("RulevVsRulev");
            else if (dynamic_cast<HumanPlayer*>(pX) || dynamic_cast<HumanPlayer*>(pO))
                learner.incrementTrainingCount("HumanVsRulev");
            else
                learner.incrementTrainingCount("StochasticVsRulev");

            learner.printLearningReport();
        }

#ifdef USE_OMP
        double elapsed = omp_get_wtime() - startTime;
#else
        double elapsed = double(std::clock() - startTime) / CLOCKS_PER_SEC;
#endif
        std::cout << "\n[TIME] Standard mode elapsed: " << elapsed << " s\n";

        delete pX;
        delete pO;
    }

    // =====================================================
    // END OF SESSION
    // =====================================================
    std::cout << "\n=== End of RulEvolution Session ===\n";
    learner.printLearningReport();

    WeightsIO::save(learner, "weights_data.txt");
    std::cout << "[INFO] Weights saved to weights_data.txt\n";

    return 0;
}