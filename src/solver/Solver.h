#pragma once
#include <vector>
#include "cube/Move.h"
#include "cube/RubiksCube.h"

class Solver {
public:
    virtual ~Solver() = default;
    virtual std::vector<Move> solve(const RubiksCube& cube) = 0;
};
