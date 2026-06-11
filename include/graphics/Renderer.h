#pragma once
#include "core/RubiksCube.h"

class Animation;

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Renders the entire Rubik's Cube in 3D, applying active rotation animations
    void renderCube(const RubiksCube& cube, const Animation& animation);

private:
    // Helper to draw a solid cubie base box (typically black)
    void drawCubieBase(float size);

    // Helper to draw a single facelet colored quad
    void drawFacelet(Face face, Colors::ColorName color);
};
