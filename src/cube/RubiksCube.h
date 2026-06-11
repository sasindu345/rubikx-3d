#pragma once
#include <vector>
#include "Cubie.h"
#include "Move.h"

class RubiksCube {
private:
    int size;
    std::vector<Cubie> cubies;

public:
    RubiksCube();
    RubiksCube(int size);
    ~RubiksCube();

    // Reset to solved state
    void reset();

    // Applies a rotation move to a slice of the cube
    void applyMove(const Move& move);

    // Checks if the cube is fully solved
    bool isSolved() const;

    // Getters
    int getSize() const { return size; }
    const std::vector<Cubie>& getCubies() const { return cubies; }
    std::vector<Cubie>& getCubies() { return cubies; }
};
