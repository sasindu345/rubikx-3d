#pragma once

class Camera {
public:
    float theta;  // Azimuth angle in degrees
    float phi;    // Elevation angle in degrees
    float radius; // Distance from target

    Camera();
    ~Camera();

    // Setup the camera view using gluLookAt
    void apply();

    // Orbit/rotate camera angles
    void orbit(float dTheta, float dPhi);

    // Zoom camera distance
    void zoom(float dRadius);
};
