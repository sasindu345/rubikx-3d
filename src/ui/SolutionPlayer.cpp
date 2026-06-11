#include "SolutionPlayer.h"
#include "../Animation.h"
#include "cube/RubiksCube.h"
#include <iostream>

SolutionPlayer::SolutionPlayer() 
    : currentIndex(0), playing(false), delayTimer(0.0f), moveDelay(0.6f) {}

SolutionPlayer::~SolutionPlayer() {}

void SolutionPlayer::setMoves(const std::vector<Move>& newMoves) {
    moves = newMoves;
    currentIndex = 0;
    playing = false;
    delayTimer = 0.0f;
}

float SolutionPlayer::getSpeed() const {
    return 1.0f / moveDelay;
}

void SolutionPlayer::adjustSpeed(float delta) {
    // Increase/decrease the delay (delta is positive to make it faster, i.e., reduce delay)
    moveDelay -= delta;
    if (moveDelay < 0.1f) moveDelay = 0.1f; // Fast limit: 10 moves per second
    if (moveDelay > 2.0f) moveDelay = 2.0f; // Slow limit: 1 move every 2 seconds
    std::cout << "Playback speed: " << getSpeed() << " moves/sec (delay: " << moveDelay << "s)" << std::endl;
}

void SolutionPlayer::stepForward(Animation& anim, RubiksCube& cube) {
    if (currentIndex >= (int)moves.size()) {
        std::cout << "Playback already at the end." << std::endl;
        return;
    }
    if (anim.isAnimating() || !anim.moveQueue.empty()) {
        return; // wait for active animation
    }
    
    std::cout << "Step Forward [" << (currentIndex + 1) << "/" << moves.size() << "]: " << moves[currentIndex].toString() << std::endl;
    anim.queueMove(moves[currentIndex]);
    currentIndex++;
}

void SolutionPlayer::stepBackward(Animation& anim, RubiksCube& cube) {
    if (currentIndex <= 0) {
        std::cout << "Playback already at the beginning." << std::endl;
        return;
    }
    if (anim.isAnimating() || !anim.moveQueue.empty()) {
        return; // wait for active animation
    }
    
    // Play the inverse of the move at (currentIndex - 1)
    Move invMove = moves[currentIndex - 1].getInverse();
    std::cout << "Step Backward [" << currentIndex << "/" << moves.size() << "]: Undoing " << moves[currentIndex - 1].toString() << " with " << invMove.toString() << std::endl;
    anim.queueMove(invMove);
    currentIndex--;
}

void SolutionPlayer::update(float deltaTime, Animation& anim, RubiksCube& cube) {
    if (!playing) return;
    
    if (currentIndex >= (int)moves.size()) {
        playing = false;
        std::cout << "Autoplay complete. Cube solved!" << std::endl;
        return;
    }
    
    if (anim.isAnimating() || !anim.moveQueue.empty()) {
        return; // wait for the animation to finish
    }
    
    delayTimer -= deltaTime;
    if (delayTimer <= 0.0f) {
        stepForward(anim, cube);
        delayTimer = moveDelay;
    }
}

void SolutionPlayer::reset() {
    currentIndex = 0;
    playing = false;
    delayTimer = 0.0f;
}
