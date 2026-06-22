#pragma once

#include "../utils/MathUtils.h"

// Sets up camera coordinates and applies the view matrix using gluLookAt
void applyCameraView(float radius, float theta, float phi);

// Sets up a perspective projection frustum
void setupPerspectiveProjection(float fov, float aspect, float zNear, float zFar);

// Sets up a flat 2D orthographic coordinate system for UI overlay rendering
void setupOrthographicProjection(int width, int height);

// Restores standard perspective projection matrices
void restoreProjection();

// Standalone manual perspective projection for Member 2 viva defense (Syllabus Math)
Vec3 manualPerspectiveProject(const Vec3& point, float fovDegrees, float aspect, float zNear, float zFar);
