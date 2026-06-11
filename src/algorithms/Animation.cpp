// ================================================
// THEORY: Computer Animation
// LECTURE: Computer Animation and Timing
// ALGORITHM: Linear interpolation (LERP) and timer-based rendering updates
// MATHEMATICS: Simple differential equation step updates
// FORMULA: 
//   theta(t) = theta_0 + speed * delta_t
// USED BY: Animation.cpp, main.cpp
// USER RESULT: Smooth face slice rotation animations and playback transitions.
// RESPONSIBLE MEMBER: Member 4
// ================================================

#include "Animation.h"

bool interpolateAngle(float& currentAngle, float speed, float deltaTime, float limit, bool& active) {
    currentAngle += speed * deltaTime;
    if (currentAngle >= limit) {
        currentAngle = 0.0f;
        active = false;
        return true; // Animation cycle completed
    }
    return false;
}
