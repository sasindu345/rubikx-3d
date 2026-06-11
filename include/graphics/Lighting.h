#pragma once

class Lighting {
public:
    // Initialize light properties (ambient, diffuse, specular) and enable lighting state
    static void init();

    // Set the positions of the light sources in the 3D scene (call inside the render loop)
    static void apply();
};
