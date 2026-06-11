#pragma once

// Applies a rotation around an axis followed by translation in 3D space
void applyCubieTransform(float rotAngle, float ax, float ay, float az, float tx, float ty, float tz);

// Restores the previous matrix transformation state
void restoreTransform();
