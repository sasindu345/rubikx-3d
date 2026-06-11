#pragma once
#include "solver/Solver.h"

class Solver4x4 : public Solver {
public:
    Solver4x4();
    ~Solver4x4() override;
    std::vector<Move> solve(const RubiksCube& cube) override;
};
