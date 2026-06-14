#pragma once

class Lighting {
public:
    // Initialize primary key light and secondary fill light
    static void init();

    // Position light sources in eye space (static default positions)
    static void apply();

    // Position light sources with an orbiting key light.
    // orbitAngleDeg rotates GL_LIGHT0 around the Y axis at the given radius.
    static void apply(float orbitAngleDeg, float orbitRadius);

    // Render a small glowing wireframe sphere at the current orbiting
    // light position so the user can see where it is in the scene.
    static void renderLightIndicator(float orbitAngleDeg, float orbitRadius);
};
