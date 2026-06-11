#pragma once
#include <string>

enum class Face {
    RIGHT = 0, // R
    LEFT,      // L
    UP,        // U
    DOWN,      // D
    FRONT,     // F
    BACK       // B
};

enum class Direction {
    CW = 0,    // Clockwise
    CCW        // Counter-Clockwise (Prime)
};

struct Move {
    Face face;
    Direction dir;
    int layer; // 0 for outermost layer, 1 for inner layer (useful for 4x4)

    Move() : face(Face::RIGHT), dir(Direction::CW), layer(0) {}
    Move(Face f, Direction d, int l = 0) : face(f), dir(d), layer(l) {}

    bool operator==(const Move& o) const {
        return face == o.face && dir == o.dir && layer == o.layer;
    }

    Move getInverse() const {
        return Move(face, (dir == Direction::CW) ? Direction::CCW : Direction::CW, layer);
    }

    std::string toString() const;
    static Move parse(const std::string& str);
};
