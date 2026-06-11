#pragma once
#include <vector>
#include "cube/Move.h"

class ReverseSolver {
public:
    // Computes solving moves by reversing and inverting the input history
    static std::vector<Move> solve(const std::vector<Move>& history);
};
