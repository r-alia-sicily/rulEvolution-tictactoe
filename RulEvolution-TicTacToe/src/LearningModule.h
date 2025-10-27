#ifndef LEARNINGMODULE_H
#define LEARNINGMODULE_H

#include "RulEvolutionRules.h"   // For RuleType and ruleToString()
#include "GameHistory.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <numeric>  // for std::accumulate
#include <iostream>

/**
 * @struct RuleStats
 * @brief Statistics and dynamic data associated with a single rule.
 */
struct RuleStats {
    double weight;     ///< Current adaptive weight of the rule
    double counter;    ///< Accumulated evidence count (positive or negative)
    double threshold;  ///< Threshold to trigger an actual update

    RuleStats(double w = 0.5, double t = 5.0)
        : weight(w), counter(0.0), threshold(t) {
    }
};

/**
 * @struct TrainingStats
 * @brief Tracks the number of games used in different training scenarios.
 */
struct TrainingStats {
    int humanVsRulev = 0;
    int stochasticVsRulev = 0;
    int rulevVsRulev = 0;
};

/**
 * @class LearningModule
 * @brief Manages the adaptive learning logic for all RulEvolution rules.
 *
 * Responsibilities:
 *  - updating rule weights according to results
 *  - maintaining stability through threshold-based evolution
 *  - normalizing weights so that their total sum is 1
 *  - exporting / printing readable learning reports
 */
class LearningModule {
public:
    explicit LearningModule(double eta);

    // --- Initialization and configuration ---
    void setRuleParameters(RuleType rule, double initialWeight, double threshold);
    void recordInitialWeights();
    void incrementTrainingCount(const std::string& gameType);
    void updateFromGame(const GameHistory& history, bool hasWon);
    void resetCounters();

    // --- Accessors ---
    std::unordered_map<RuleType, double> getWeights() const;
    std::vector<double> exportWeightVector() const;
    void compareWeightVectors(const std::vector<double>& before,
        const std::vector<double>& after);
    void printLearningReport() const;

    // --- Manual adjustments and data persistence ---
    bool setWeight(RuleType rule, double newWeight);                ///< Manually set a weight
    double getThreshold(RuleType rule) const;                       ///< Retrieve rule threshold
    void normalizeWeights(double minW = 0.0, double maxW = 1.0);    ///< Keep weights within [min,max] and normalize sum to 1

private:
    double learningRate;  ///< Learning rate
    std::unordered_map<RuleType, RuleStats> ruleData;   ///< Data for each rule
    std::unordered_map<RuleType, double> initialWeights;///< Snapshot of initial weights
    TrainingStats trainingStats;                        ///< Counters for training sessions
};

#endif // LEARNINGMODULE_H