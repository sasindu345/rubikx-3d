/*
--------------------------------------------------
Module: Polygon and Area Filling

Purpose:
Provides rasterization utilities for drawing solid filled panels and borders
for 2D dashboard elements.

Graphics Concepts:
- Area Filling
- Scan-Line Polygon Fill (simulated via primitive drawing)
- Border / Edge Outline Tracing

Mathematics:
- Interpolation of bounding points on screen grids
- View coordinate discretization

Responsibilities:
- Rasterizing filled quadrilaterals for transparent and solid panels
- Rasterizing loops for border layouts
--------------------------------------------------
*/

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
