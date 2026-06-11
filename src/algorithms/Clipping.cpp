// ================================================
// THEORY: 2D Viewing and Clipping
// LECTURE: Line Clipping and Viewing Volumes
// ALGORITHM: Cohen-Sutherland Line Clipping and Frustum bounds checks
// MATHEMATICS: Outcode bitwise tests and frustum plane line intersections
// FORMULA: 
//   Outcodes: bit 0: Left, bit 1: Right, bit 2: Bottom, bit 3: Top
//   Outcode0 AND Outcode1 != 0 => Trivial Reject
//   Outcode0 OR Outcode1 == 0 => Trivial Accept
// USED BY: HUD.cpp, main.cpp
// USER RESULT: Correct raster boundaries mapping and volume culling.
// RESPONSIBLE MEMBER: Member 5
// ================================================

#include "Clipping.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void configureViewport(int x, int y, int w, int h) {
    glViewport(x, y, w, h);
}

bool checkClippingVolume(float z, float zNear, float zFar) {
    // Check if the coordinate lies inside the perspective frustum range
    return (z >= -zFar && z <= -zNear);
}

// 4-bit Outcodes for Cohen-Sutherland boundaries
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

int computeOutCode(float x, float y, const ClipRect& rect) {
    int code = INSIDE;

    if (x < rect.xMin)      code |= LEFT;
    else if (x > rect.xMax) code |= RIGHT;
    if (y < rect.yMin)      code |= BOTTOM;
    else if (y > rect.yMax) code |= TOP;

    return code;
}

bool clipCohenSutherland(float& x0, float& y0, float& x1, float& y1, const ClipRect& rect) {
    int code0 = computeOutCode(x0, y0, rect);
    int code1 = computeOutCode(x1, y1, rect);
    bool accept = false;

    while (true) {
        if ((code0 | code1) == 0) {
            // Trivial accept
            accept = true;
            break;
        } else if (code0 & code1) {
            // Trivial reject
            break;
        } else {
            // Calculate intersection point
            float x, y;
            int outcodeOut = code0 ? code0 : code1;

            if (outcodeOut & TOP) {
                x = x0 + (x1 - x0) * (rect.yMax - y0) / (y1 - y0);
                y = rect.yMax;
            } else if (outcodeOut & BOTTOM) {
                x = x0 + (x1 - x0) * (rect.yMin - y0) / (y1 - y0);
                y = rect.yMin;
            } else if (outcodeOut & RIGHT) {
                y = y0 + (y1 - y0) * (rect.xMax - x0) / (x1 - x0);
                x = rect.xMax;
            } else if (outcodeOut & LEFT) {
                y = y0 + (y1 - y0) * (rect.xMin - x0) / (x1 - x0);
                x = rect.xMin;
            }

            if (outcodeOut == code0) {
                x0 = x;
                y0 = y;
                code0 = computeOutCode(x0, y0, rect);
            } else {
                x1 = x;
                y1 = y;
                code1 = computeOutCode(x1, y1, rect);
            }
        }
    }
    return accept;
}
