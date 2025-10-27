#ifndef WEIGHTSIO_H
#define WEIGHTSIO_H

#include "LearningModule.h"
#include <string>

class WeightsIO {
public:
    // Load weights from file (returns true if successful)
    static bool load(LearningModule& learner, const std::string& filename);

    // Save current weights to file
    static void save(const LearningModule& learner, const std::string& filename);

private:
    static bool isValidRuleId(int id) {
        return id == (int)RULE_BLOCK ||
            id == (int)RULE_PREPARATION ||
            id == (int)RULE_CENTER ||
            id == (int)RULE_CORNER ||
            id == (int)RULE_SIDE;
    }
};

#endif // WEIGHTSIO_H