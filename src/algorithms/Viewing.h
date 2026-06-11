#pragma once

// Sets up camera coordinates and applies the view matrix using gluLookAt
void applyCameraView(float radius, float theta, float phi);

// Sets up a perspective projection frustum
void setupPerspectiveProjection(float fov, float aspect, float zNear, float zFar);

// Sets up a flat 2D orthographic coordinate system for UI overlay rendering
void setupOrthographicProjection(int width, int height);

// Restores standard perspective projection matrices
void restoreProjection();
