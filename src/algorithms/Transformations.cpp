/*
 * Module: 3D Transformations
 * Purpose: Provides functions for applying coordinate transformations.
 */

#include "Transformations.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Applies rotation and translation on the OpenGL matrix stack.
void applyCubieTransform(float rotAngle, float ax, float ay, float az, float tx, float ty, float tz) {
    glPushMatrix();
    if (rotAngle != 0.0f) {
        glRotatef(rotAngle, ax, ay, az);
    }
    glTranslatef(tx, ty, tz);
}

// Restores matrix transformation state.
void restoreTransform() {
    glPopMatrix();
}

// Applies scaling offset to translation coordinates for explosion view.
void applyExplosionOffset(float& tx, float& ty, float& tz, float factor) {
    if (factor > 0.001f) {
        tx *= (1.0f + factor);
        ty *= (1.0f + factor);
        tz *= (1.0f + factor);
    }
}

// Creates a 4x4 translation matrix using homogeneous coordinates.
Mat4 manualTranslationMatrix(float tx, float ty, float tz) {
    Mat4 mat;
    mat.m[0] = 1.0f; mat.m[4] = 0.0f; mat.m[8]  = 0.0f; mat.m[12] = tx;
    mat.m[1] = 0.0f; mat.m[5] = 1.0f; mat.m[9]  = 0.0f; mat.m[13] = ty;
    mat.m[2] = 0.0f; mat.m[6] = 0.0f; mat.m[10] = 1.0f; mat.m[14] = tz;
    mat.m[3] = 0.0f; mat.m[7] = 0.0f; mat.m[11] = 0.0f; mat.m[15] = 1.0f;
    return mat;
}

// Creates a 4x4 rotation matrix around an arbitrary axis using Rodrigues' rotation formula.
Mat4 manualRotationMatrix(float angleDegrees, float ax, float ay, float az) {
    Mat4 mat;
    float rad = angleDegrees * 3.14159265f / 180.0f;
    float c = std::cos(rad);
    float s = std::sin(rad);
    float omc = 1.0f - c;

    float len = std::sqrt(ax * ax + ay * ay + az * az);
    if (len > 1e-6f) {
        ax /= len; ay /= len; az /= len;
    } else {
        return Mat4::identity();
    }

    mat.m[0] = c + ax * ax * omc;
    mat.m[1] = ay * ax * omc + az * s;
    mat.m[2] = az * ax * omc - ay * s;
    mat.m[3] = 0.0f;

    mat.m[4] = ax * ay * omc - az * s;
    mat.m[5] = c + ay * ay * omc;
    mat.m[6] = az * ay * omc + ax * s;
    mat.m[7] = 0.0f;

    mat.m[8] = ax * az * omc + ay * s;
    mat.m[9] = ay * az * omc - ax * s;
    mat.m[10] = c + az * az * omc;
    mat.m[11] = 0.0f;

    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;

    return mat;
}

// Creates a 4x4 scaling matrix using scale factors.
Mat4 manualScalingMatrix(float sx, float sy, float sz) {
    Mat4 mat;
    mat.m[0] = sx;   mat.m[4] = 0.0f; mat.m[8]  = 0.0f;  mat.m[12] = 0.0f;
    mat.m[1] = 0.0f; mat.m[5] = sy;   mat.m[9]  = 0.0f;  mat.m[13] = 0.0f;
    mat.m[2] = 0.0f; mat.m[6] = 0.0f; mat.m[10] = sz;    mat.m[14] = 0.0f;
    mat.m[3] = 0.0f; mat.m[7] = 0.0f; mat.m[11] = 0.0f;  mat.m[15] = 1.0f;
    return mat;
}
