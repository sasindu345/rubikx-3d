/*
 * Module: 3D Viewing and Projections
 * Purpose: Manages camera look-at coordinates transformations and projection pipeline setups.
 */

#include "Viewing.h"
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Computes camera coordinates from spherical rotation inputs and applies look-at transformation.
void applyCameraView(float radius, float theta, float phi) {
    float thetaRad = theta * 3.14159265f / 180.0f;
    float phiRad = phi * 3.14159265f / 180.0f;

    float cx = radius * std::cos(phiRad) * std::sin(thetaRad);
    float cy = radius * std::sin(phiRad);
    float cz = radius * std::cos(phiRad) * std::cos(thetaRad);

    gluLookAt(cx, cy, cz, 
              0.0f, 0.0f, 0.0f, 
              0.0f, 1.0f, 0.0f);
}

// Configures perspective projection matrices on the projection stack.
void setupPerspectiveProjection(float fov, float aspect, float zNear, float zFar) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspect, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Configures parallel 2D orthographic projection coordinate system.
void setupOrthographicProjection(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0); // Origin at top-left
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

// Restores standard perspective matrices states.
void restoreProjection() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// Applies perspective projection equations to convert a 3D point into projected coordinates.
Vec3 manualPerspectiveProject(const Vec3& point, float fovDegrees, float aspect, float zNear, float zFar) {
    float fovRad = fovDegrees * 3.14159265f / 180.0f;
    float f = 1.0f / std::tan(fovRad * 0.5f);

    float xc = point.x * (f / aspect);
    float yc = point.y * f;
    float zc = point.z * ((zFar + zNear) / (zNear - zFar)) + ((2.0f * zFar * zNear) / (zNear - zFar));
    float wc = -point.z;

    if (std::abs(wc) > 1e-6f) {
        return Vec3(xc / wc, yc / wc, zc / wc);
    }
    return Vec3(xc, yc, zc);
}
