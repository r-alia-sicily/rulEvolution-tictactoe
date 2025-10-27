#define _CRT_SECURE_NO_WARNINGS
#include "WeightsIO.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <numeric>   // necessario per std::accumulate


static std::string nowStamp() {
    std::time_t t = std::time(nullptr);
    char buf[64]{};
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buf);
}

bool WeightsIO::load(LearningModule& learner, const std::string& filename) {
    std::ifstream in(filename);
    if (!in.good()) return false;

    std::string line;
    int loaded = 0;

    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        int id;
        double w;
        if (!(iss >> id >> w)) continue;
        if (!isValidRuleId(id)) continue;

        learner.setWeight(static_cast<RuleType>(id), std::max(0.0, std::min(1.0, w)));
        ++loaded;
    }

    return (loaded > 0);
}

void WeightsIO::save(const LearningModule& learner, const std::string& filename) {
    auto weights = learner.getWeights();

    std::ofstream out(filename, std::ios::trunc);
    if (!out.good()) return;

    out << "# RulEvolution TicTacToe Weights v1\n";
    out << "# Saved: " << nowStamp() << "\n";
    out << "# Format: <rule_id> <weight>\n";
    out << "# RULE_WIN (0) is absolute and not saved\n";

    auto emit = [&](RuleType rule) {
        auto it = weights.find(rule);
        if (it != weights.end())
            out << (int)rule << " " << std::fixed << std::setprecision(3) << it->second << "\n";
        };

    emit(RULE_BLOCK);
    emit(RULE_PREPARATION);
    emit(RULE_CENTER);
    emit(RULE_CORNER);
    emit(RULE_SIDE);
}