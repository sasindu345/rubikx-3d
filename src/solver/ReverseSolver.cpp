#include "ReverseSolver.h"

std::vector<Move> ReverseSolver::solve(const std::vector<Move>& history) {
    std::vector<Move> solution;
    for (auto it = history.rbegin(); it != history.rend(); ++it) {
        solution.push_back(it->getInverse());
    }
    return solution;
}
