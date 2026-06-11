#pragma once
#include <vector>
#include "core/Move.h"
#include "core/RubiksCube.h"

class Solver {
public:
    virtual ~Solver() = default;
    virtual std::vector<Move> solve(const RubiksCube& cube) = 0;
};
