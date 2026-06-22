/*
 * Module: Visible Surface Detection
 * Purpose: Manages depth testing, back-face culling flags, and surface normals.
 */

#include "Visibility.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Enables hardware depth testing comparison checks.
void enableDepthTesting() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

// Enables hardware back-face culling culling checks.
void enableBackFaceCulling() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

// Configures current surface normals vectors.
void setupSurfaceNormal(float nx, float ny, float nz) {
    glNormal3f(nx, ny, nz);
}

// Determines whether a polygon is facing toward or away from the camera.
bool manualBackFaceCull(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& viewDir) {
    Vec3 e1 = v1 - v0;
    Vec3 e2 = v2 - v0;
    Vec3 normal = e1.cross(e2).normalized();
    float dotVal = normal.dot(viewDir);
    return (dotVal < 0.0f);
}
