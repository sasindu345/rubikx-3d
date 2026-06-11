#pragma once
#include "core/RubiksCube.h"

class CubeFactory {
public:
    // Factory method to construct and return a RubiksCube of specified size
    static RubiksCube create(int size);
};
