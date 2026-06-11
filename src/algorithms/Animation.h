#pragma once

// Interpolates a rotation angle towards a target limit based on velocity and delta time
bool interpolateAngle(float& currentAngle, float speed, float deltaTime, float limit, bool& active);
