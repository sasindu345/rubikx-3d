// ================================================
// THEORY: Shading & Colour Interpolation
// LECTURE: Visible Surface Detection and Shading
// ALGORITHM: Gouraud Shading (per-vertex colour interpolation)
// MATHEMATICS: Barycentric interpolation of vertex attributes
// FORMULA:
//   C(x,y) = alpha * C_v0 + beta * C_v1 + gamma * C_v2
//   where alpha + beta + gamma = 1 (barycentric coordinates)
//   The GPU linearly interpolates vertex colours across each
//   rasterized scanline, producing a smooth gradient.
// USED BY: main.cpp (background rendering)
// USER RESULT: Smooth cosmic gradient background behind the 3D scene.
// RESPONSIBLE MEMBER: Member 2
// ================================================

#include "Shading.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void renderGradientBackground(int width, int height,
                              float r0, float g0, float b0,
                              float r1, float g1, float b1,
                              float r2, float g2, float b2,
                              float r3, float g3, float b3) {
    // Switch to 2D orthographic projection for the full-screen quad
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0); // origin top-left

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Disable depth/lighting — the gradient sits behind everything
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);

    // Draw a full-screen quad with per-vertex colours.
    // The fixed-function pipeline performs Gouraud interpolation
    // automatically across the polygon interior.
    glBegin(GL_QUADS);
    glColor3f(r0, g0, b0);  // Top-left
    glVertex2i(0, 0);
    glColor3f(r1, g1, b1);  // Top-right
    glVertex2i(width, 0);
    glColor3f(r2, g2, b2);  // Bottom-right
    glVertex2i(width, height);
    glColor3f(r3, g3, b3);  // Bottom-left
    glVertex2i(0, height);
    glEnd();

    // Restore state
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
