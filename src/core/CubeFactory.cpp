#include "core/CubeFactory.h"

RubiksCube CubeFactory::create(int size) {
    return RubiksCube(size);
}
