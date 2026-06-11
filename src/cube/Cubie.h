#pragma once
#include "Colors.h"
#include "Move.h"

class Cubie {
public:
    int ix, iy, iz; // Grid coordinates, 0 to size-1
    Colors::ColorName colors[6]; // Colors for each of the 6 faces (ordered by Face enum index)

    Cubie();
    Cubie(int x, int y, int z, int size);
    ~Cubie();

    // Rotates coordinates and updates face colors accordingly
    void rotate(Face face, Direction dir, int size);
};
