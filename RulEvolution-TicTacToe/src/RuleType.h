#ifndef RULETYPE_H
#define RULETYPE_H

#include <string>

/**
 * @enum RuleType
 * @brief Identifiers for the different RulEvolution rules.
 */
enum RuleType {
    RULE_WIN = 0,          // Winning move (absolute rule)
    RULE_BLOCK,            // Blocking move
    RULE_CENTER,           // Center preference
    RULE_CORNER,           // Corner preference
    RULE_SIDE,             // Side preference
    RULE_PREPARATION       // Preparing a future winning configuration
};

/**
 * @brief Convert a rule enum to a human-readable string.
 */
inline std::string ruleToString(RuleType rule) {
    switch (rule) {
    case RULE_WIN:          return "0 WIN";
    case RULE_BLOCK:        return "1 BLOCK";
    case RULE_CENTER:       return "2 CENTER";
    case RULE_CORNER:       return "3 CORNER";
    case RULE_SIDE:         return "4 SIDE";
    case RULE_PREPARATION:  return "5 PREPARATION";
    default:                return "? UNKNOWN";
    }
}

#endif // RULETYPE_H
