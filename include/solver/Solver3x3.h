#pragma once
#include "solver/Solver.h"

class Solver3x3 : public Solver {
public:
    Solver3x3();
    ~Solver3x3() override;
    std::vector<Move> solve(const RubiksCube& cube) override;
};
