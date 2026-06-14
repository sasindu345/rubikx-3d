/*
--------------------------------------------------
Module: 3D Camera Controller

Purpose:
Manages camera tracking, orbit angles, zoom distance, and coordinate conversions
for navigating around the 3D Rubik's Cube.

Graphics Concepts:
- 3D Viewing Transformation
- Orbit Controls
- Gimbal Lock Prevention

Mathematics:
- Spherical Coordinates (Theta, Phi, Radius)
- Boundary clamping (phi in [-89, 89], theta in [0, 360])

Responsibilities:
- Orbiting camera azimuth and elevation angles
- Zooming camera distance in and out
- Applying the view transformations via applyCameraView()
--------------------------------------------------
*/

#include "Camera.h"
#include "algorithms/Algorithms.h"
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

Camera::Camera() : theta(45.0f), phi(30.0f), radius(10.0f) {}
Camera::~Camera() {}

void Camera::apply() {
    applyCameraView(radius, theta, phi);
}

void Camera::orbit(float dTheta, float dPhi) {
    theta += dTheta;
    phi += dPhi;

    // Clamp elevation to avoid gimbal lock at the poles
    if (phi > 89.0f) phi = 89.0f;
    if (phi < -89.0f) phi = -89.0f;

    // Keep azimuth within [0, 360] range
    if (theta > 360.0f) theta -= 360.0f;
    if (theta < 0.0f) theta += 360.0f;
}

void Camera::zoom(float dRadius) {
    radius += dRadius;
    if (radius < 3.0f) radius = 3.0f;
    if (radius > 30.0f) radius = 30.0f;
}
