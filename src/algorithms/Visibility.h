#pragma once

#include "../utils/MathUtils.h"

// Enables depth buffering tests to perform hidden surface removal (VSD)
void enableDepthTesting();

// Enables back-face culling to discard triangles facing away from viewer
void enableBackFaceCulling();

// Sets the current surface normal for lighting reflection calculations
void setupSurfaceNormal(float nx, float ny, float nz);

// Standalone manual back-face culling check for Member 3 viva defense (Syllabus Math)
bool manualBackFaceCull(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& viewDir);
