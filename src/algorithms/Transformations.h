#pragma once

// Applies a rotation around an axis followed by translation in 3D space
void applyCubieTransform(float rotAngle, float ax, float ay, float az, float tx, float ty, float tz);

// Restores the previous matrix transformation state
void restoreTransform();

// Applies an outward explosion offset to a translation vector.
// Scales each component by (1 + factor) to push cubies radially
// outward from the centre, revealing internal cube structure.
void applyExplosionOffset(float& tx, float& ty, float& tz, float factor);
