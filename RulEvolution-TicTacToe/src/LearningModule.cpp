// ================================================================
//  LearningModule.cpp — Parallel-friendly (OpenMP Optional, MSVC-safe)
//  Notes:
//    - updateFromGame() kept sequential for determinism (threshold logic).
//    - normalizeWeights() uses index-based loops over a refs vector
//      to be valid under MSVC OpenMP on ARM64.
// ================================================================
#include "LearningModule.h"
#include "RulEvolutionRules.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <vector>
#include <functional> // std::reference_wrapper

#ifdef USE_OMP
#include <omp.h>
#endif

LearningModule::LearningModule(double eta)
    : learningRate(eta) {
}

// --- Initialization and configuration --------------------------------------

void LearningModule::setRuleParameters(RuleType rule, double initialWeight, double threshold) {
    ruleData[rule] = RuleStats(initialWeight, threshold);
}

void LearningModule::recordInitialWeights() {
    initialWeights.clear();
    for (const auto& pair : ruleData)
        initialWeights[pair.first] = pair.second.weight;
}

void LearningModule::incrementTrainingCount(const std::string& gameType) {
    if (gameType == "HumanVsRulev")        trainingStats.humanVsRulev++;
    else if (gameType == "StochasticVsRulev") trainingStats.stochasticVsRulev++;
    else if (gameType == "RulevVsRulev")     trainingStats.rulevVsRulev++;
}

// --- Learning core ----------------------------------------------------------

/**
 * @brief Update rule weights after a completed game.
 *        Counters accumulate evidence; a weight is changed only when its
 *        counter crosses ±threshold. Kept sequential to preserve exact
 *        per-move threshold/reset semantics (deterministic behavior).
 */
void LearningModule::updateFromGame(const GameHistory& history, bool hasWon) {
    std::cout << "\n=== LEARNING UPDATE START ===\n";

    for (const auto& moveRecord : history.moves) {
        for (auto rule : moveRecord.rules) {
            auto& stats = ruleData[rule];
            double oldWeight = stats.weight;

            // Evidence accumulation
            stats.counter += (hasWon ? 1.0 : -1.0);

            // Thresholded updates
            if (stats.counter >= stats.threshold) {
                stats.weight += learningRate;
                stats.counter = 0.0;
            }
            else if (stats.counter <= -stats.threshold) {
                stats.weight -= learningRate;
                stats.counter = 0.0;
            }

            // Clamp to [0, 1]
            stats.weight = std::max(0.0, std::min(1.0, stats.weight));

            // Trace
            std::cout << "Rule " << std::setw(2) << (int)rule << " (" << ruleToString(rule) << ")"
                << " | Old: " << std::fixed << std::setprecision(3) << oldWeight
                << " -> New: " << stats.weight
                << " | Counter: " << stats.counter
                << " | Threshold: " << stats.threshold << "\n";
        }
    }

    normalizeWeights();  // keep global consistency
    std::cout << "=== LEARNING UPDATE END ===\n\n";
}

/**
 * @brief Normalize all weights: clamp to [minW,maxW] and ensure sum = 1.
 *        MSVC-OpenMP-safe: iterate with index over a refs vector.
 */
void LearningModule::normalizeWeights(double minW, double maxW) {
    // Build an indexable list of references to RuleStats to allow OpenMP index loops.
    std::vector<std::reference_wrapper<RuleStats>> refs;
    refs.reserve(ruleData.size());
    for (auto& kv : ruleData) refs.push_back(kv.second);

    // 1) Clamp and accumulate total sum (single pass)
    double sum = 0.0;

#ifdef USE_OMP
#pragma omp parallel for reduction(+:sum)
#endif
    for (int i = 0; i < static_cast<int>(refs.size()); ++i) {
        RuleStats& rs = refs[i].get();
        rs.weight = std::max(minW, std::min(maxW, rs.weight));
        sum += rs.weight;
    }

    // 2) Normalize if sum > 0 (separate pass to avoid dividing during reduction)
    if (sum > 0.0) {
#ifdef USE_OMP
#pragma omp parallel for
#endif
        for (int i = 0; i < static_cast<int>(refs.size()); ++i) {
            RuleStats& rs = refs[i].get();
            rs.weight /= sum;
        }
    }
}

// --- Utilities --------------------------------------------------------------

void LearningModule::resetCounters() {
    for (auto& pair : ruleData)
        pair.second.counter = 0.0;
}

bool LearningModule::setWeight(RuleType rule, double newWeight) {
    auto it = ruleData.find(rule);
    if (it == ruleData.end()) {
        ruleData[rule] = RuleStats(newWeight, 5.0); // default threshold
        return true;
    }
    it->second.weight = std::max(0.0, std::min(1.0, newWeight));
    return true;
}

double LearningModule::getThreshold(RuleType rule) const {
    auto it = ruleData.find(rule);
    return (it != ruleData.end()) ? it->second.threshold : 0.0;
}

// --- Accessors / reporting --------------------------------------------------

std::unordered_map<RuleType, double> LearningModule::getWeights() const {
    std::unordered_map<RuleType, double> weights;
    for (const auto& pair : ruleData)
        weights[pair.first] = pair.second.weight;
    return weights;
}

std::vector<double> LearningModule::exportWeightVector() const {
    std::vector<double> vec;
    vec.reserve(ruleData.size());
    for (const auto& pair : ruleData)
        vec.push_back(pair.second.weight);
    return vec;
}

void LearningModule::compareWeightVectors(const std::vector<double>& before,
    const std::vector<double>& after) {
    std::cout << "=== WEIGHT COMPARISON ===\n";
    size_t n = std::min(before.size(), after.size());
    for (size_t i = 0; i < n; ++i)
        std::cout << "Rule " << i
        << ": before = " << before[i]
        << ", after = " << after[i]
        << ", delta = " << (after[i] - before[i]) << "\n";
    std::cout << "=========================\n";
}

void LearningModule::printLearningReport() const {
    std::cout << "\n========== LEARNING REPORT ==========\n\n";

    std::cout << std::left << std::setw(18) << "Rule (Name)"
        << std::setw(10) << "Init"
        << std::setw(10) << "Current"
        << std::setw(10) << "Delta"
        << std::setw(10) << "Counter"
        << std::setw(10) << "Threshold" << "\n";
    std::cout << "---------------------------------------------------------------\n";

    double total = 0.0;
    for (const auto& pair : ruleData) {
        RuleType rule = pair.first;
        const RuleStats& stats = pair.second;
        double init = (initialWeights.count(rule) ? initialWeights.at(rule) : stats.weight);
        double delta = stats.weight - init;
        total += stats.weight;

        std::ostringstream label;
        label << ruleToString(rule);

        std::cout << std::left << std::setw(18) << label.str()
            << std::setw(10) << std::fixed << std::setprecision(3) << init
            << std::setw(10) << stats.weight
            << std::setw(10) << delta
            << std::setw(10) << stats.counter
            << std::setw(10) << stats.threshold << "\n";
    }

    std::cout << "---------------------------------------------------------------\n";
    std::cout << "Total weight sum = " << std::fixed << std::setprecision(3) << total << "\n\n";

    std::cout << "Training data used:\n"
        << "  Human vs RulEvolution:      " << trainingStats.humanVsRulev << "\n"
        << "  Stochastic vs RulEvolution: " << trainingStats.stochasticVsRulev << "\n"
        << "  RulEvolution vs RulEvolution: " << trainingStats.rulevVsRulev << "\n"
        << "=====================================\n\n";
}