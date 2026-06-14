#include "PatternLibrary.h"

std::vector<CubePattern> PatternLibrary::getPatterns(int size) {
    std::vector<CubePattern> p;

    // 1. Checkerboard (Available for all sizes, generated dynamically)
    std::vector<std::string> checkerboardMoves;
    // X axis
    for (int layer = 0; layer < size; layer += 2) {
        checkerboardMoves.push_back(Move(Face::RIGHT, Direction::CW, layer).toString() + "2");
    }
    // Y axis
    for (int layer = 0; layer < size; layer += 2) {
        checkerboardMoves.push_back(Move(Face::UP, Direction::CW, layer).toString() + "2");
    }
    // Z axis
    for (int layer = 0; layer < size; layer += 2) {
        checkerboardMoves.push_back(Move(Face::FRONT, Direction::CW, layer).toString() + "2");
    }
    p.push_back({"Checkerboard", checkerboardMoves});

    // 2. Superflip (Available for size >= 3)
    if (size >= 3) {
        p.push_back({"Superflip", {"U","R2","F","B","R","B2","R","U2","L","B2","R","U'","D'","R2","F","R'","L","B2","U2","F2"}});
    }

    // 3. Cube in Cube (Available for size >= 2)
    if (size >= 2) {
        p.push_back({"Cube in Cube", {"U","F","R2","F2","D","B2","D'","L2","D","B","L","U2","D","F2"}});
    }

    return p;
}
