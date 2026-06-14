/*
--------------------------------------------------
Module: Font Rasterization

Purpose:
Draws bitmap font character arrays onto screen coordinates for text display.

Graphics Concepts:
- Bitmap Font Rasterization
- Pixel-Aligned Screen Layouts

Mathematics:
- Coordinate mapping
- Text-width horizontal step accumulation

Responsibilities:
- Rendering bitmap character strings at defined (x, y) coordinates
- Setting custom color tracking for text elements
--------------------------------------------------
*/

#include "Rasterization.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void rasterizeFont(int x, int y, const std::string& str, void* font, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2i(x, y);
    for (char c : str) {
        glutBitmapCharacter(font, c);
    }
}
