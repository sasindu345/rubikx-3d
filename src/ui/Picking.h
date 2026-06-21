#pragma once
#include "../cube/RubiksCube.h"
#include "../Camera.h"
#include "../Animation.h"

// Performs OpenGL color-based picking to identify which cubie coordinate (ix, iy, iz)
// is located under the given mouse coordinates.
// Returns true if a cubie was hit, false otherwise.
bool pickCubie(int mouseX, int mouseY, const RubiksCube& cube, Camera& camera, 
               const Animation& animation, float explosionFactor, 
               int windowWidth, int windowHeight, 
               int& outIx, int& outIy, int& outIz);
