// ================================================
// THEORY: 3D Transformations
// LECTURE: 3D Transformation and Viewing
// ALGORITHM: Translation and Rotation Matrices
// MATHEMATICS: Homogeneous coordinate system calculations
// FORMULA: 
//   T(tx, ty, tz) = [1 0 0 tx; 0 1 0 ty; 0 0 1 tz; 0 0 0 1]
//   Rx(theta)     = [1 0 0 0; 0 cos(theta) -sin(theta) 0; 0 sin(theta) cos(theta) 0; 0 0 0 1]
// USED BY: Renderer.cpp
// USER RESULT: Rotates and translates individual cubies to form the Rubik's Cube structure.
// RESPONSIBLE MEMBER: Member 1
// ================================================

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
