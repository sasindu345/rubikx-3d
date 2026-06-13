#include "PatternLibrary.h"

std::vector<CubePattern> PatternLibrary::getPatterns3x3() {
    std::vector<CubePattern> p;
    p.push_back({"Checkerboard", {"F2","B2","U2","D2","L2","R2"}});
    p.push_back({"Superflip", {"U","R2","F","B","R","B2","R","U2","L","B2","R","U'","D'","R2","F","R'","L","B2","U2","F2"}});
    p.push_back({"Cube in Cube", {"U","F","R2","F2","D","B2","D'","L2","D","B","L","U2","D","F2"}});
    return p;
}
