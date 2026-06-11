#pragma once

// Configures the screen space display viewport
void configureViewport(int x, int y, int w, int h);

// Checks if an object's z-depth coordinate lies within the visible near/far clipping volume
bool checkClippingVolume(float z, float zNear, float zFar);

// Cohen-Sutherland line clipping algorithms parameters structure and evaluation stub
struct ClipRect {
    float xMin, yMin, xMax, yMax;
};
int computeOutCode(float x, float y, const ClipRect& rect);
bool clipCohenSutherland(float& x0, float& y0, float& x1, float& y1, const ClipRect& rect);
