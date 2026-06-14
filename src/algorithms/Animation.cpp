/*
--------------------------------------------------
Module: Animation Utilities

Purpose:
Provides linear interpolation utility functions to compute smooth
frame-to-frame angle transitions for face rotation animations.

Graphics Concepts:
- Keyframe Animation
- Transformation Animation
- Linear Interpolation (LERP)

Mathematics:
- First-order Euler step updates
- Angle accumulation and bounding checks

Responsibilities:
- Interpolating the rotation angle based on elapsed time and animation speed
- Updating active animation state when limit values are reached
--------------------------------------------------
*/

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
