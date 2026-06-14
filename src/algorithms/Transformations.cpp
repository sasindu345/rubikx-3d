/*
--------------------------------------------------
Module: 3D Transformations

Purpose:
Provides translation and rotation transformations for positioning
and animating individual cubies in 3D space.

Graphics Concepts:
- 3D Translation
- 3D Rotation
- Composite Transformations

Mathematics:
- Homogeneous Coordinate Matrices
- Rotation Matrix Calculations
- Translation Vector Calculations

Responsibilities:
- Pushing matrix transformations for slice and block rotators
- Performing translation matrix mappings
- Restoring transform matrix state
--------------------------------------------------
*/

#include "Transformations.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void applyCubieTransform(float rotAngle, float ax, float ay, float az, float tx, float ty, float tz) {
    glPushMatrix();
    if (rotAngle != 0.0f) {
        glRotatef(rotAngle, ax, ay, az);
    }
    glTranslatef(tx, ty, tz);
}

void restoreTransform() {
    glPopMatrix();
}

void applyExplosionOffset(float& tx, float& ty, float& tz, float factor) {
    if (factor > 0.001f) {
        tx *= (1.0f + factor);
        ty *= (1.0f + factor);
        tz *= (1.0f + factor);
    }
}
