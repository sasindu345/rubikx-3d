// ================================================
// THEORY: 3D Viewing
// LECTURE: 3D Viewing and Projection
// ALGORITHM: Camera coordinates view and projection transformations
// MATHEMATICS: Spherical coordinate frames and frustum definitions
// FORMULA: 
//   gluLookAt(cx, cy, cz, tx, ty, tz, ux, uy, uz)
//   gluPerspective(fov, aspect, zNear, zFar)
// USED BY: Camera.cpp, main.cpp, HUD.cpp
// USER RESULT: Correct perspective rendering of the 3D scene and 2D HUD overlays.
// RESPONSIBLE MEMBER: Member 2
// ================================================

#include "Viewing.h"
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

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

void setupPerspectiveProjection(float fov, float aspect, float zNear, float zFar) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspect, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void setupOrthographicProjection(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0); // Origin at top-left
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void restoreProjection() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
