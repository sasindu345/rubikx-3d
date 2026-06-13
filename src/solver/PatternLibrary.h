#pragma once
#include <vector>
#include <string>
#include "cube/Move.h"

struct CubePattern {
    std::string name;
    std::vector<std::string> algorithm; // notation strings, e.g. "R", "U'", "F2"
};

class PatternLibrary {
public:
    static std::vector<CubePattern> getPatterns3x3();
};
