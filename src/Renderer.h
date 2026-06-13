#pragma once
#include "cube/RubiksCube.h"

class Animation;

enum class RenderMode {
    SOLID,
    WIREFRAME,
    TEXTURED
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Alpha Blending / Glass Cube mode toggle
    // When true: cubie bases are semi-transparent (glass effect), stickers remain opaque
    // Uses GL_BLEND + Painter's Algorithm (back-to-front sort) for correct compositing
    bool alphaBlending = false;

    // Active rendering mode (Solid, Wireframe, Textured)
    RenderMode renderMode = RenderMode::SOLID;

    // Renders the entire Rubik's Cube in 3D, applying active rotation animations
    void renderCube(const RubiksCube& cube, const Animation& animation);

private:
    // Texture ID for textured mode
    unsigned int textureId = 0;

    // Generates the procedural sticker texture lazily
    void initTexture();

    // Helper to draw a solid cubie base box (typically black)
    // alpha parameter controls transparency (1.0 = opaque, <1.0 = transparent glass)
    void drawCubieBase(float size, float alpha = 1.0f);

    // Helper to draw a single facelet colored quad
    // alpha parameter for sticker transparency
    void drawFacelet(Face face, Colors::ColorName color, float alpha = 1.0f);

    // Apply transform for a cubie (shared between opaque and transparent passes)
    // Returns true if a rotation was applied for the animation pass
    void applyCubieRenderTransform(const struct CubieTransformData& data);
};

