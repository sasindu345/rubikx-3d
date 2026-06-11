#include "core/RubiksCube.h"

RubiksCube::RubiksCube() : RubiksCube(3) {}

RubiksCube::RubiksCube(int s) : size(s) {
    reset();
}

RubiksCube::~RubiksCube() {}

void RubiksCube::reset() {
    cubies.clear();
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            for (int z = 0; z < size; ++z) {
                cubies.push_back(Cubie(x, y, z, size));
            }
        }
    }
}

void RubiksCube::applyMove(const Move& move) {
    for (auto& cubie : cubies) {
        bool match = false;
        switch (move.face) {
            case Face::RIGHT: match = (cubie.ix == size - 1 - move.layer); break;
            case Face::LEFT:  match = (cubie.ix == move.layer); break;
            case Face::UP:    match = (cubie.iy == size - 1 - move.layer); break;
            case Face::DOWN:  match = (cubie.iy == move.layer); break;
            case Face::FRONT: match = (cubie.iz == size - 1 - move.layer); break;
            case Face::BACK:  match = (cubie.iz == move.layer); break;
        }
        if (match) {
            cubie.rotate(move.face, move.dir, size);
        }
    }
}

bool RubiksCube::isSolved() const {
    int r = static_cast<int>(Face::RIGHT);
    int l = static_cast<int>(Face::LEFT);
    int u = static_cast<int>(Face::UP);
    int d = static_cast<int>(Face::DOWN);
    int f = static_cast<int>(Face::FRONT);
    int b = static_cast<int>(Face::BACK);

    // 1. Right Face
    {
        Colors::ColorName faceColor = Colors::NONE;
        for (const auto& cubie : cubies) {
            if (cubie.ix == size - 1) {
                Colors::ColorName c = cubie.colors[r];
                if (c == Colors::NONE) return false;
                if (faceColor == Colors::NONE) faceColor = c;
                else if (c != faceColor) return false;
            }
        }
    }

    // 2. Left Face
    {
        Colors::ColorName faceColor = Colors::NONE;
        for (const auto& cubie : cubies) {
            if (cubie.ix == 0) {
                Colors::ColorName c = cubie.colors[l];
                if (c == Colors::NONE) return false;
                if (faceColor == Colors::NONE) faceColor = c;
                else if (c != faceColor) return false;
            }
        }
    }

    // 3. Up Face
    {
        Colors::ColorName faceColor = Colors::NONE;
        for (const auto& cubie : cubies) {
            if (cubie.iy == size - 1) {
                Colors::ColorName c = cubie.colors[u];
                if (c == Colors::NONE) return false;
                if (faceColor == Colors::NONE) faceColor = c;
                else if (c != faceColor) return false;
            }
        }
    }

    // 4. Down Face
    {
        Colors::ColorName faceColor = Colors::NONE;
        for (const auto& cubie : cubies) {
            if (cubie.iy == 0) {
                Colors::ColorName c = cubie.colors[d];
                if (c == Colors::NONE) return false;
                if (faceColor == Colors::NONE) faceColor = c;
                else if (c != faceColor) return false;
            }
        }
    }

    // 5. Front Face
    {
        Colors::ColorName faceColor = Colors::NONE;
        for (const auto& cubie : cubies) {
            if (cubie.iz == size - 1) {
                Colors::ColorName c = cubie.colors[f];
                if (c == Colors::NONE) return false;
                if (faceColor == Colors::NONE) faceColor = c;
                else if (c != faceColor) return false;
            }
        }
    }

    // 6. Back Face
    {
        Colors::ColorName faceColor = Colors::NONE;
        for (const auto& cubie : cubies) {
            if (cubie.iz == 0) {
                Colors::ColorName c = cubie.colors[b];
                if (c == Colors::NONE) return false;
                if (faceColor == Colors::NONE) faceColor = c;
                else if (c != faceColor) return false;
            }
        }
    }

    return true;
}
