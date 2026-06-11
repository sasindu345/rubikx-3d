#include "graphics/Animation.h"

Animation::Animation() : currentAngle(0.0f), speed(450.0f), active(false) {}

Animation::~Animation() {}

void Animation::queueMove(const Move& move) {
    moveQueue.push(move);
    if (!active) {
        start(moveQueue.front());
        moveQueue.pop();
    }
}

void Animation::start(const Move& move) {
    currentMove = move;
    currentAngle = 0.0f;
    active = true;
}

bool Animation::update(float deltaTime) {
    if (!active) {
        if (!moveQueue.empty()) {
            start(moveQueue.front());
            moveQueue.pop();
        } else {
            return false;
        }
    }

    currentAngle += speed * deltaTime;
    if (currentAngle >= 90.0f) {
        currentAngle = 0.0f;
        active = false;
        return true; // Move completed
    }
    return false;
}

bool Animation::getCubieRotation(int ix, int iy, int iz, int size, float& outAngle, float& ax, float& ay, float& az) const {
    if (!active) return false;

    bool inSlice = false;
    switch (currentMove.face) {
        case Face::RIGHT: inSlice = (ix == size - 1 - currentMove.layer); break;
        case Face::LEFT:  inSlice = (ix == currentMove.layer); break;
        case Face::UP:    inSlice = (iy == size - 1 - currentMove.layer); break;
        case Face::DOWN:  inSlice = (iy == currentMove.layer); break;
        case Face::FRONT: inSlice = (iz == size - 1 - currentMove.layer); break;
        case Face::BACK:  inSlice = (iz == currentMove.layer); break;
    }

    if (!inSlice) return false;

    ax = 0.0f;
    ay = 0.0f;
    az = 0.0f;
    float sign = 1.0f;

    switch (currentMove.face) {
        case Face::RIGHT:
            ax = 1.0f;
            sign = (currentMove.dir == Direction::CW) ? 1.0f : -1.0f;
            break;
        case Face::LEFT:
            ax = 1.0f;
            sign = (currentMove.dir == Direction::CW) ? -1.0f : 1.0f;
            break;
        case Face::UP:
            ay = 1.0f;
            sign = (currentMove.dir == Direction::CW) ? -1.0f : 1.0f;
            break;
        case Face::DOWN:
            ay = 1.0f;
            sign = (currentMove.dir == Direction::CW) ? 1.0f : -1.0f;
            break;
        case Face::FRONT:
            az = 1.0f;
            sign = (currentMove.dir == Direction::CW) ? -1.0f : 1.0f;
            break;
        case Face::BACK:
            az = 1.0f;
            sign = (currentMove.dir == Direction::CW) ? 1.0f : -1.0f;
            break;
    }

    outAngle = currentAngle * sign;
    return true;
}
