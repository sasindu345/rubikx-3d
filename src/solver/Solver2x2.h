#pragma once
#include "Solver.h"

class Solver2x2 : public Solver {
public:
    Solver2x2();
    ~Solver2x2() override;
    std::vector<Move> solve(const RubiksCube& cube) override;
};
