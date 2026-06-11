// ================================================
// THEORY: Polygon and Area Filling
// LECTURE: Area Filling and Rasterization
// ALGORITHM: Scan Line Polygon Fill (GPU level rasterization)
// MATHEMATICS: Linear interpolation of edge boundaries on screen grids
// FORMULA: 
//   Horizontal spans filled between sorted active edge pairs.
// USED BY: HUD.cpp
// USER RESULT: Solid background panels and border layouts.
// RESPONSIBLE MEMBER: Member 3
// ================================================

#include "Filling.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void rasterizeQuad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glVertex2i(x3, y3);
    glVertex2i(x4, y4);
    glEnd();
}

void rasterizeBorder(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, float r, float g, float b, float a, float lineWidth) {
    glLineWidth(lineWidth);
    glColor4f(r, g, b, a);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glVertex2i(x3, y3);
    glVertex2i(x4, y4);
    glEnd();
    glLineWidth(1.0f);
}
