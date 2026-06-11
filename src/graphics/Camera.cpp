#include "graphics/Camera.h"
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline float toRad(float deg) {
    return deg * static_cast<float>(M_PI) / 180.0f;
}

Camera::Camera() : theta(45.0f), phi(30.0f), radius(10.0f) {}

Camera::~Camera() {}

void Camera::apply() {
    float thetaRad = toRad(theta);
    float phiRad = toRad(phi);

    float cx = radius * std::cos(phiRad) * std::sin(thetaRad);
    float cy = radius * std::sin(phiRad);
    float cz = radius * std::cos(phiRad) * std::cos(thetaRad);

    // Up vector is along positive Y axis. 
    // phi is clamped to [-89, 89], so we never go beyond the poles.
    gluLookAt(cx, cy, cz, 
              0.0f, 0.0f, 0.0f, 
              0.0f, 1.0f, 0.0f);
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
