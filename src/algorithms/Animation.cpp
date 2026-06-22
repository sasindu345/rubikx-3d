/*
 * Module: Animation Utilities
 * Purpose: Provides linear interpolation utilities for updating rotation angles.
 */

#include "Animation.h"

// Interpolates rotation angles to create smooth cube animations.
bool interpolateAngle(float& currentAngle, float speed, float deltaTime, float limit, bool& active) {
    currentAngle += speed * deltaTime;
    if (currentAngle >= limit) {
        currentAngle = 0.0f;
        active = false;
        return true;
    }
    return false;
}
