// ================================================
// THEORY: Visible Surface Detection
// LECTURE: Visible Surface Detection and Shading
// ALGORITHM: Back-Face Culling and Depth Testing (Z-Buffer)
// MATHEMATICS: Normal vector dot products and polygon wind orientation tests
// FORMULA: 
//   Dot product N . V < 0  => Visible
//   Dot product N . V >= 0 => Culled
// USED BY: Renderer.cpp, main.cpp
// USER RESULT: Correct occlusion sorting and exclusion of internal/back faces.
// RESPONSIBLE MEMBER: Member 3
// ================================================

#include "Visibility.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void enableDepthTesting() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void enableBackFaceCulling() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void setupSurfaceNormal(float nx, float ny, float nz) {
    glNormal3f(nx, ny, nz);
}
