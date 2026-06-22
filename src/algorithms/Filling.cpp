/*
 * Module: Filling Utilities
 * Purpose: Provides region filling and border tracing functionalities.
 */

#include "Filling.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Draws a solid filled 2D quadrilateral (implied scanline fill by graphics hardware).
void rasterizeQuad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glVertex2i(x3, y3);
    glVertex2i(x4, y4);
    glEnd();
}

// Draws an unfilled wireframe border outline.
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

// Performs scanline polygon filling over a vertical range of scanlines.
void manualScanLineFill(int yStart, int yEnd, float r, float g, float b) {
    for (int y = yStart; y <= yEnd; ++y) {
        int xStart = 10;
        int xEnd = 100;
        glBegin(GL_LINES);
        glColor3f(r, g, b);
        glVertex2i(xStart, y);
        glVertex2i(xEnd, y);
        glEnd();
    }
}
