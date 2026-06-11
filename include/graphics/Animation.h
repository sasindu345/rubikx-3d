#pragma once
#include "core/Move.h"
#include <queue>

class Animation {
public:
    Move currentMove;
    float currentAngle; // ranges from 0.0f to 90.0f
    float speed;        // speed in degrees per second (default 360.0f)
    bool active;
    std::queue<Move> moveQueue;

    Animation();
    ~Animation();

    // Queues a move for execution
    void queueMove(const Move& move);

    // Forces a move to start immediately (bypassing the queue)
    void start(const Move& move);

    // Updates animation state by deltaTime (in seconds)
    // Returns true if a move just completed in this frame
    bool update(float deltaTime);

    // Checks if animation is in progress
    bool isAnimating() const { return active; }

    // Returns true if the cubie is in the active slice, and sets OpenGL rotation arguments
    bool getCubieRotation(int ix, int iy, int iz, int size, float& outAngle, float& ax, float& ay, float& az) const;
};
