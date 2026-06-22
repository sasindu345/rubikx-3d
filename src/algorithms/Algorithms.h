#pragma once
#include <string>
#include "../utils/MathUtils.h"

// Transformations
void applyCubieTransform(float rotAngle, float ax, float ay, float az, float tx, float ty, float tz);
void restoreTransform();
Mat4 manualTranslationMatrix(float tx, float ty, float tz);
Mat4 manualRotationMatrix(float angleDegrees, float ax, float ay, float az);
Mat4 manualScalingMatrix(float sx, float sy, float sz);

// Viewing
void applyCameraView(float radius, float theta, float phi);
void setupPerspectiveProjection(float fov, float aspect, float zNear, float zFar);
void setupOrthographicProjection(int width, int height);
void restoreProjection();
Vec3 manualPerspectiveProject(const Vec3& point, float fovDegrees, float aspect, float zNear, float zFar);

// Visibility
void enableDepthTesting();
void enableBackFaceCulling();
void setupSurfaceNormal(float nx, float ny, float nz);
bool manualBackFaceCull(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& viewDir);

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
void manualScanLineFill(int yStart, int yEnd, float r, float g, float b);

// Shading
void renderGradientBackground(int width, int height, float r0, float g0, float b0, float r1, float g1, float b1, float r2, float g2, float b2, float r3, float g3, float b3);
