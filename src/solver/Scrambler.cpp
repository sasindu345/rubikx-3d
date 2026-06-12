#include "Scrambler.h"
#include <random>

Scrambler::Scrambler() {}
Scrambler::~Scrambler() {}

std::vector<Move> Scrambler::generateScramble(int size, int steps) {
    std::vector<Move> scrambleMoves;
    if (steps <= 0) return scrambleMoves;

    // Use mt19937 generator seeded with random_device
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> faceDist(0, 5);
    std::uniform_int_distribution<int> dirDist(0, 1);
    
    // For larger cubes (>= 4x4), we allow inner slice turns
    int maxLayer = (size >= 4) ? (size - 1) / 2 : 0;
    std::uniform_int_distribution<int> layerDist(0, maxLayer);

    Face lastFace = static_cast<Face>(-1);

    for (int i = 0; i < steps; ++i) {
        Face f;
        do {
            f = static_cast<Face>(faceDist(rng));
        } while (f == lastFace); // Filter out redundant consecutive moves on the same face

        Direction d = static_cast<Direction>(dirDist(rng));
        int l = layerDist(rng);

        scrambleMoves.push_back(Move(f, d, l));
        lastFace = f;
    }

    return scrambleMoves;
}
