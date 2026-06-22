/*
 * Module: Clipping Utilities
 * Purpose: Provides line clipping algorithms to clip vector coordinate bounds.
 */

#include "Clipping.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Configures screen viewport mapping.
void configureViewport(int x, int y, int w, int h) {
    glViewport(x, y, w, h);
}

// Checks if coordinates are within viewing frustum.
bool checkClippingVolume(float z, float zNear, float zFar) {
    return (z >= -zFar && z <= -zNear);
}

const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

// Computes region outcode based on viewport boundaries.
int computeOutCode(float x, float y, const ClipRect& rect) {
    int code = INSIDE;

    if (x < rect.xMin)      code |= LEFT;
    else if (x > rect.xMax) code |= RIGHT;
    if (y < rect.yMin)      code |= BOTTOM;
    else if (y > rect.yMax) code |= TOP;

    return code;
}

// Performs Cohen-Sutherland clipping using region outcodes.
bool clipCohenSutherland(float& x0, float& y0, float& x1, float& y1, const ClipRect& rect) {
    int code0 = computeOutCode(x0, y0, rect);
    int code1 = computeOutCode(x1, y1, rect);
    bool accept = false;

    while (true) {
        if ((code0 | code1) == 0) {
            accept = true;
            break;
        } else if (code0 & code1) {
            break;
        } else {
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
