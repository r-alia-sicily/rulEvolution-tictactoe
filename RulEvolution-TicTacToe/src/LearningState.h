#ifndef LEARNING_STATE_H
#define LEARNING_STATE_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>  // Needed for std::accumulate

/**
 * @struct LearningState
 * @brief Holds the adaptive internal state of a RulEvolutionPlayer.
 *
 * This structure stores the dynamic learning parameters,
 * mainly the rule weights that evolve through training.
 */
struct LearningState {
    /// Current rule weights (aligned with enum RuleType)
    std::vector<double> weights;

    /// Default constructor: creates an empty state
    LearningState() = default;

    /**
     * @brief Initialize weights for a given number of rules with a default value.
     * @param numRules Number of rules expected.
     * @param defaultWeight Initial value for each rule (default = 0.5).
     */
    void initialize(size_t numRules, double defaultWeight = 0.5) {
        weights.assign(numRules, defaultWeight);
        normalize();
    }

    /**
     * @brief Update current weights with new values (size-checked).
     * @param newWeights The updated vector of rule weights.
     */
    void update(const std::vector<double>& newWeights) {
        if (newWeights.size() != weights.size()) {
            std::cout << "[WARN] LearningState::update(): size mismatch ("
                << newWeights.size() << " vs " << weights.size() << ")\n";
        }
        weights = newWeights;
        normalize();
    }

    /**
     * @brief Normalize all weights so their sum equals 1 (simple linear normalization).
     */
    void normalize() {
        double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
        if (sum > 0.0) {
            for (double& w : weights)
                w /= sum;
        }
    }

    /**
     * @brief Clamp all weights to the range [0, 1].
     */
    void clamp() {
        for (double& w : weights)
            w = std::max(0.0, std::min(1.0, w));
    }

    /**
     * @brief Print current weights (for debugging and diagnostics).
     */
    void print() const {
        double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
        std::cout << "[LearningState] Weights: ";
        for (double w : weights)
            std::cout << std::fixed << std::setprecision(3) << w << " ";
        std::cout << "(sum=" << std::fixed << std::setprecision(3) << sum << ")\n";
    }
};

#endif // LEARNING_STATE_H