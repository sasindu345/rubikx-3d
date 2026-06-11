#pragma once

class Lighting {
public:
    // Initialize primary key light and secondary fill light
    static void init();

    // Position light sources in eye space
    static void apply();
};
