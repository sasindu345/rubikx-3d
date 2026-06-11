// ================================================
// THEORY: 2D Font Rasterization
// LECTURE: Rasterization and Text Overlays
// ALGORITHM: Character bitmap plotting
// MATHEMATICS: Mapping coordinate matrices to bitmap pixel strides
// FORMULA: 
//   Pixel(x, y) = CharacterGlyphMap[char][pixel_index]
// USED BY: HUD.cpp
// USER RESULT: High-performance 2D font text drawing.
// RESPONSIBLE MEMBER: Member 5
// ================================================

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
