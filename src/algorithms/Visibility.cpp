/*
--------------------------------------------------
Module: Visible Surface Detection

Purpose:
Determines surface visibility and performs occlusion culling to ensure
correct rendering order and discard invisible geometries.

Graphics Concepts:
- Visible Surface Detection (VSD)
- Back-Face Culling
- Depth Testing (Z-Buffer)

Mathematics:
- Surface Normal Vectors
- Eye View Vector Dot Products
- Polygon Winding Order (CCW/CW)

Responsibilities:
- Enabling and configuring the depth buffer function (GL_LESS)
- Enabling and configuring back-face culling (GL_CULL_FACE, GL_BACK)
- Defining surface normals to support correct back-face detection
--------------------------------------------------
*/

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
