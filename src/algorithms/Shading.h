#pragma once

// Renders a full-screen gradient quad using per-vertex colour interpolation
// (Gouraud Shading). Each corner is assigned a distinct colour; the GPU
// linearly interpolates across the polygon interior.
void renderGradientBackground(int width, int height,
                              float r0, float g0, float b0,   // top-left
                              float r1, float g1, float b1,   // top-right
                              float r2, float g2, float b2,   // bottom-right
                              float r3, float g3, float b3);  // bottom-left
