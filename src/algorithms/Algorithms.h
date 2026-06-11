#pragma once
#include <string>

// Transformations
void applyCubieTransform(float rotAngle, float ax, float ay, float az, float tx, float ty, float tz);
void restoreTransform();

// Viewing
void applyCameraView(float radius, float theta, float phi);
void setupPerspectiveProjection(float fov, float aspect, float zNear, float zFar);
void setupOrthographicProjection(int width, int height);
void restoreProjection();

// Visibility
void enableDepthTesting();
void enableBackFaceCulling();
void setupSurfaceNormal(float nx, float ny, float nz);

// Animation
bool interpolateAngle(float& currentAngle, float speed, float deltaTime, float limit, bool& active);

// Clipping
void configureViewport(int x, int y, int w, int h);
bool checkClippingVolume(float z, float zNear, float zFar);
struct ClipRect {
    float xMin, yMin, xMax, yMax;
};
int computeOutCode(float x, float y, const ClipRect& rect);
bool clipCohenSutherland(float& x0, float& y0, float& x1, float& y1, const ClipRect& rect);

// Rasterization
void rasterizeFont(int x, int y, const std::string& str, void* font, float r, float g, float b);

// Filling
void rasterizeQuad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, float r, float g, float b, float a);
void rasterizeBorder(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, float r, float g, float b, float a, float lineWidth);
