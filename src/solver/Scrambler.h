#pragma once
#include <vector>
#include "cube/Move.h"

class Scrambler {
public:
    Scrambler();
    ~Scrambler();

    // Generates a list of random moves for a cube of given size
    // Ensures no consecutive moves on the same face (redundancy check)
    static std::vector<Move> generateScramble(int size, int steps);
};
