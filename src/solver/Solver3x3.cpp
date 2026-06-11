#include "solver/Solver3x3.h"
#include <iostream>
#include <sstream>
#include <cctype>

Solver3x3::Solver3x3() {}
Solver3x3::~Solver3x3() {}

void Solver3x3::doMove(RubiksCube& cube, Face face, Direction dir, int layer) {
    Direction finalDir = dir;
    if (face == Face::RIGHT || face == Face::LEFT) {
        finalDir = (dir == Direction::CW) ? Direction::CCW : Direction::CW;
    }
    Move m(face, finalDir, layer);
    std::cout << "      [doMove] " << m.toString() << std::endl;
    cube.applyMove(m);
    solutionMoves.push_back(m);
}

void Solver3x3::doMove(RubiksCube& cube, const Move& m) {
    Direction finalDir = m.dir;
    if (m.face == Face::RIGHT || m.face == Face::LEFT) {
        finalDir = (m.dir == Direction::CW) ? Direction::CCW : Direction::CW;
    }
    Move finalMove(m.face, finalDir, m.layer);
    std::cout << "      [doMove] " << finalMove.toString() << " (from const Move&)" << std::endl;
    cube.applyMove(finalMove);
    solutionMoves.push_back(finalMove);
}

void Solver3x3::doAlg(RubiksCube& cube, const std::string& alg) {
    std::stringstream ss(alg);
    std::string token;
    while (ss >> token) {
        if (!token.empty()) {
            bool isDouble = false;
            std::string parseToken = token;
            if (token.back() == '2') {
                isDouble = true;
                parseToken = token.substr(0, token.size() - 1);
            }
            Move m = Move::parse(parseToken);
            doMove(cube, m);
            if (isDouble) {
                doMove(cube, m);
            }
        }
    }
}

int Solver3x3::findEdge(const RubiksCube& cube, Colors::ColorName c1, Colors::ColorName c2) const {
    for (size_t i = 0; i < cube.getCubies().size(); ++i) {
        const auto& cubie = cube.getCubies()[i];
        int matches = 0;
        int coloredCount = 0;
        for (int f = 0; f < 6; ++f) {
            if (cubie.colors[f] != Colors::NONE) {
                coloredCount++;
                if (cubie.colors[f] == c1 || cubie.colors[f] == c2) {
                    matches++;
                }
            }
        }
        if (matches == 2 && coloredCount == 2) return i;
    }
    return -1;
}

int Solver3x3::findCorner(const RubiksCube& cube, Colors::ColorName c1, Colors::ColorName c2, Colors::ColorName c3) const {
    for (size_t i = 0; i < cube.getCubies().size(); ++i) {
        const auto& cubie = cube.getCubies()[i];
        int matches = 0;
        int coloredCount = 0;
        for (int f = 0; f < 6; ++f) {
            if (cubie.colors[f] != Colors::NONE) {
                coloredCount++;
                if (cubie.colors[f] == c1 || cubie.colors[f] == c2 || cubie.colors[f] == c3) {
                    matches++;
                }
            }
        }
        if (matches == 3 && coloredCount == 3) return i;
    }
    return -1;
}

Face Solver3x3::getFaceFromColor(Colors::ColorName color) const {
    if (color == Colors::RED) return Face::RIGHT;
    if (color == Colors::ORANGE) return Face::LEFT;
    if (color == Colors::WHITE) return Face::UP;
    if (color == Colors::YELLOW) return Face::DOWN;
    if (color == Colors::GREEN) return Face::FRONT;
    if (color == Colors::BLUE) return Face::BACK;
    return Face::UP;
}

bool Solver3x3::isRightOf(Face fSide, Face fBot) const {
    if (fSide == Face::FRONT) return fBot == Face::RIGHT;
    if (fSide == Face::RIGHT) return fBot == Face::BACK;
    if (fSide == Face::BACK) return fBot == Face::LEFT;
    if (fSide == Face::LEFT) return fBot == Face::FRONT;
    return false;
}

std::string Solver3x3::getInsertionAlg(Face fSide, bool toRight) const {
    if (fSide == Face::FRONT) return toRight ? "D' R' D R D F D' F'" : "D L D' L' D' F' D F";
    if (fSide == Face::RIGHT) return toRight ? "D' B' D B D R D' R'" : "D F D' F' D' R' D R";
    if (fSide == Face::BACK)  return toRight ? "D' L' D L D B D' B'" : "D R D' R' D' B' D B";
    if (fSide == Face::LEFT)  return toRight ? "D' F' D F D L D' L'" : "D B D' B' D' L' D L";
    return "";
}

Face Solver3x3::getRightFaceOfBack(Face fBack) const {
    if (fBack == Face::BACK) return Face::RIGHT;
    if (fBack == Face::LEFT) return Face::BACK;
    if (fBack == Face::FRONT) return Face::LEFT;
    if (fBack == Face::RIGHT) return Face::FRONT;
    return Face::RIGHT;
}

void Solver3x3::applyUPerf(RubiksCube& cube, Face fBack, bool cw) {
    Face fR = getRightFaceOfBack(fBack);
    if (cw) {
        // R2 D R D R' D' R' D' R' D R'
        doMove(cube, fR, Direction::CW); doMove(cube, fR, Direction::CW);
        doMove(cube, Face::DOWN, Direction::CW);
        doMove(cube, fR, Direction::CW);
        doMove(cube, Face::DOWN, Direction::CW);
        doMove(cube, fR, Direction::CCW);
        doMove(cube, Face::DOWN, Direction::CCW);
        doMove(cube, fR, Direction::CCW);
        doMove(cube, Face::DOWN, Direction::CCW);
        doMove(cube, fR, Direction::CCW);
        doMove(cube, Face::DOWN, Direction::CW);
        doMove(cube, fR, Direction::CCW);
    } else {
        // R D' R D R D R D' R' D' R2
        doMove(cube, fR, Direction::CW);
        doMove(cube, Face::DOWN, Direction::CCW);
        doMove(cube, fR, Direction::CW);
        doMove(cube, Face::DOWN, Direction::CW);
        doMove(cube, fR, Direction::CW);
        doMove(cube, Face::DOWN, Direction::CW);
        doMove(cube, fR, Direction::CW);
        doMove(cube, Face::DOWN, Direction::CCW);
        doMove(cube, fR, Direction::CCW);
        doMove(cube, Face::DOWN, Direction::CCW);
        doMove(cube, fR, Direction::CW); doMove(cube, fR, Direction::CW);
    }
}

void Solver3x3::solveWhiteCross(RubiksCube& cube) {
    struct EdgeTarget {
        Colors::ColorName sideColor;
        Face sideFace;
        int targetX, targetY, targetZ;
    };

    EdgeTarget targets[4] = {
        { Colors::GREEN, Face::FRONT, 1, 2, 2 },
        { Colors::RED, Face::RIGHT, 2, 2, 1 },
        { Colors::BLUE, Face::BACK, 1, 2, 0 },
        { Colors::ORANGE, Face::LEFT, 0, 2, 1 }
    };

    for (const auto& target : targets) {
        while (true) {
            int idx = findEdge(cube, Colors::WHITE, target.sideColor);
            const auto& cubie = cube.getCubies()[idx];
            
            // Check if solved
            if (cubie.ix == target.targetX && cubie.iy == 2 && cubie.iz == target.targetZ) {
                if (cubie.colors[static_cast<int>(Face::UP)] == Colors::WHITE) {
                    break; // Solved!
                }
            }
            
            // If in UP layer, rotate 180 degrees to bring to DOWN layer
            if (cubie.iy == 2) {
                if (cubie.ix == 1 && cubie.iz == 2) doAlg(cube, "F2");
                else if (cubie.ix == 2 && cubie.iz == 1) doAlg(cube, "R2");
                else if (cubie.ix == 1 && cubie.iz == 0) doAlg(cube, "B2");
                else if (cubie.ix == 0 && cubie.iz == 1) doAlg(cube, "L2");
                continue;
            }
            
            // If in middle layer, eject to DOWN layer
            if (cubie.iy == 1) {
                if (cubie.ix == 2 && cubie.iz == 2) doAlg(cube, "F D F'");
                else if (cubie.ix == 2 && cubie.iz == 0) doAlg(cube, "R D R'");
                else if (cubie.ix == 0 && cubie.iz == 0) doAlg(cube, "B D B'");
                else if (cubie.ix == 0 && cubie.iz == 2) doAlg(cube, "L D L'");
                continue;
            }
            
            // Now in bottom layer, rotate D until below home position
            while (true) {
                int currIdx = findEdge(cube, Colors::WHITE, target.sideColor);
                if (cube.getCubies()[currIdx].ix == target.targetX && cube.getCubies()[currIdx].iz == target.targetZ) {
                    break;
                }
                doMove(cube, Face::DOWN, Direction::CW);
            }
            
            // Align orientation and place it in first layer
            int finalIdx = findEdge(cube, Colors::WHITE, target.sideColor);
            if (cube.getCubies()[finalIdx].colors[static_cast<int>(Face::DOWN)] == Colors::WHITE) {
                if (target.sideFace == Face::FRONT) doAlg(cube, "F2");
                else if (target.sideFace == Face::RIGHT) doAlg(cube, "R2");
                else if (target.sideFace == Face::BACK) doAlg(cube, "B2");
                else if (target.sideFace == Face::LEFT) doAlg(cube, "L2");
            } else {
                if (target.sideFace == Face::FRONT) doAlg(cube, "D R D F'");
                else if (target.sideFace == Face::RIGHT) doAlg(cube, "D R B R'");
                else if (target.sideFace == Face::BACK) doAlg(cube, "D L D B'");
                else if (target.sideFace == Face::LEFT) doAlg(cube, "D F L' F'");
            }
            break;
        }
    }
}

void Solver3x3::solveWhiteCorners(RubiksCube& cube) {
    struct CornerTarget {
        Colors::ColorName c1, c2;
        int targetX, targetY, targetZ;
    };

    CornerTarget targets[4] = {
        { Colors::GREEN, Colors::RED, 2, 2, 2 },
        { Colors::GREEN, Colors::ORANGE, 0, 2, 2 },
        { Colors::BLUE, Colors::RED, 2, 2, 0 },
        { Colors::BLUE, Colors::ORANGE, 0, 2, 0 }
    };

    for (const auto& target : targets) {
        while (true) {
            int idx = findCorner(cube, Colors::WHITE, target.c1, target.c2);
            const auto& cubie = cube.getCubies()[idx];
            
            if (cubie.ix == target.targetX && cubie.iy == 2 && cubie.iz == target.targetZ) {
                if (cubie.colors[static_cast<int>(Face::UP)] == Colors::WHITE) {
                    break; // Solved!
                }
            }
            
            // If in UP layer, eject to DOWN layer
            if (cubie.iy == 2) {
                if (cubie.ix == 2 && cubie.iz == 2) doAlg(cube, "R' D' R");
                else if (cubie.ix == 0 && cubie.iz == 2) doAlg(cube, "F' D' F");
                else if (cubie.ix == 2 && cubie.iz == 0) doAlg(cube, "B' D' B");
                else if (cubie.ix == 0 && cubie.iz == 0) doAlg(cube, "L' D' L");
                continue;
            }
            
            // Now in bottom layer, rotate D until below home position
            while (true) {
                int currIdx = findCorner(cube, Colors::WHITE, target.c1, target.c2);
                if (cube.getCubies()[currIdx].ix == target.targetX && cube.getCubies()[currIdx].iz == target.targetZ) {
                    break;
                }
                doMove(cube, Face::DOWN, Direction::CW);
            }
            
            // Apply sexy moves until oriented
            while (true) {
                int finalIdx = findCorner(cube, Colors::WHITE, target.c1, target.c2);
                const auto& finalCubie = cube.getCubies()[finalIdx];
                if (finalCubie.ix == target.targetX && finalCubie.iy == 2 && finalCubie.iz == target.targetZ &&
                    finalCubie.colors[static_cast<int>(Face::UP)] == Colors::WHITE) {
                    break;
                }
                if (target.targetX == 2 && target.targetZ == 2) doAlg(cube, "R' D' R D");
                else if (target.targetX == 0 && target.targetZ == 2) doAlg(cube, "F' D' F D");
                else if (target.targetX == 2 && target.targetZ == 0) doAlg(cube, "B' D' B D");
                else if (target.targetX == 0 && target.targetZ == 0) doAlg(cube, "L' D' L D");
            }
            break;
        }
    }
}

void Solver3x3::solveMiddleLayer(RubiksCube& cube) {
    struct MiddleEdgeTarget {
        Colors::ColorName c1, c2;
        int targetX, targetY, targetZ;
    };

    MiddleEdgeTarget targets[4] = {
        { Colors::GREEN, Colors::RED, 2, 1, 2 },
        { Colors::GREEN, Colors::ORANGE, 0, 1, 2 },
        { Colors::BLUE, Colors::RED, 2, 1, 0 },
        { Colors::BLUE, Colors::ORANGE, 0, 1, 0 }
    };

    for (int t = 0; t < 4; ++t) {
        const auto& target = targets[t];
        std::cout << "  - [Middle Layer] Solving edge: " << Colors::getColorNameString(target.c1) << "-" << Colors::getColorNameString(target.c2) << std::endl;
        
        int iters = 0;
        while (true) {
            iters++;
            if (iters > 20) {
                std::cout << "  - [Middle Layer] ERROR: stuck in loop for target " << t << "!" << std::endl;
                break;
            }
            
            int idx = findEdge(cube, target.c1, target.c2);
            const auto& cubie = cube.getCubies()[idx];
            
            std::cout << "    Current pos: (" << cubie.ix << ", " << cubie.iy << ", " << cubie.iz << ")" << std::endl;
            
            if (cubie.ix == target.targetX && cubie.iy == 1 && cubie.iz == target.targetZ) {
                Face fSide = getFaceFromColor(target.c1);
                std::cout << "    Already at target, checking color orientation..." << std::endl;
                if (cubie.colors[static_cast<int>(fSide)] == target.c1) {
                    std::cout << "    Already solved and oriented!" << std::endl;
                    break; // Solved and oriented!
                }
            }
            
            // If in middle layer, eject it
            if (cubie.iy == 1) {
                std::cout << "    Ejecting from middle layer position (" << cubie.ix << ", 1, " << cubie.iz << ")..." << std::endl;
                if (cubie.ix == 2 && cubie.iz == 2) doAlg(cube, "D' R' D R D F D' F'");
                else if (cubie.ix == 0 && cubie.iz == 2) doAlg(cube, "D L D' L' D' F' D F");
                else if (cubie.ix == 2 && cubie.iz == 0) doAlg(cube, "D' B' D B D R D' R'");
                else if (cubie.ix == 0 && cubie.iz == 0) doAlg(cube, "D B D' B' D' L' D L");
                continue;
            }
            
            // Now in bottom layer, rotate D until aligned
            std::cout << "    In bottom layer, checking alignment..." << std::endl;
            while (true) {
                int currIdx = findEdge(cube, target.c1, target.c2);
                const auto& currCubie = cube.getCubies()[currIdx];
                
                Face sideFace = Face::UP;
                Colors::ColorName sideColor = Colors::NONE;
                for (int f = 0; f < 6; ++f) {
                    if (f != static_cast<int>(Face::DOWN) && currCubie.colors[f] != Colors::NONE) {
                        sideFace = static_cast<Face>(f);
                        sideColor = currCubie.colors[f];
                        break;
                    }
                }
                
                Face centerFace = getFaceFromColor(sideColor);
                std::cout << "      sideFace: " << static_cast<int>(sideFace) << ", centerFace: " << static_cast<int>(centerFace) << " (color: " << Colors::getColorNameString(sideColor) << ")" << std::endl;
                if (sideFace == centerFace) {
                    Colors::ColorName botColor = (sideColor == target.c1) ? target.c2 : target.c1;
                    Face botFace = getFaceFromColor(botColor);
                    bool toRight = isRightOf(centerFace, botFace);
                    std::cout << "      Aligned! Inserting centerFace=" << static_cast<int>(centerFace) << ", toRight=" << toRight << std::endl;
                    doAlg(cube, getInsertionAlg(centerFace, toRight));
                    break;
                }
                std::cout << "      Not aligned. Rotating D..." << std::endl;
                doMove(cube, Face::DOWN, Direction::CW);
            }
            break;
        }
        if (iters > 20) break;
    }
}

void Solver3x3::solveYellowCross(RubiksCube& cube) {
    while (true) {
        bool fYellow = (cube.getCubies()[findEdge(cube, Colors::YELLOW, Colors::GREEN)].colors[static_cast<int>(Face::DOWN)] == Colors::YELLOW);
        bool rYellow = (cube.getCubies()[findEdge(cube, Colors::YELLOW, Colors::RED)].colors[static_cast<int>(Face::DOWN)] == Colors::YELLOW);
        bool bYellow = (cube.getCubies()[findEdge(cube, Colors::YELLOW, Colors::BLUE)].colors[static_cast<int>(Face::DOWN)] == Colors::YELLOW);
        bool lYellow = (cube.getCubies()[findEdge(cube, Colors::YELLOW, Colors::ORANGE)].colors[static_cast<int>(Face::DOWN)] == Colors::YELLOW);

        int count = (fYellow ? 1 : 0) + (rYellow ? 1 : 0) + (bYellow ? 1 : 0) + (lYellow ? 1 : 0);
        if (count == 4) break;

        if (count == 0) {
            doAlg(cube, "F R D R' D' F'");
        } else if (count == 2) {
            if (lYellow && rYellow) {
                doAlg(cube, "F R D R' D' F'");
            } else if (fYellow && bYellow) {
                doAlg(cube, "D F R D R' D' F'");
            } else if (bYellow && lYellow) {
                doAlg(cube, "F D R D' R' F'");
            } else if (lYellow && fYellow) {
                doAlg(cube, "D F D R D' R' F'");
            } else if (fYellow && rYellow) {
                doAlg(cube, "D2 F D R D' R' F'");
            } else if (rYellow && bYellow) {
                doAlg(cube, "D' F D R D' R' F'");
            }
        }
    }
}

void Solver3x3::orientYellowCorners(RubiksCube& cube) {
    for (int i = 0; i < 4; ++i) {
        while (true) {
            int idx = -1;
            for (size_t c = 0; c < cube.getCubies().size(); ++c) {
                if (cube.getCubies()[c].ix == 2 && cube.getCubies()[c].iy == 0 && cube.getCubies()[c].iz == 2) {
                    idx = c;
                    break;
                }
            }
            if (cube.getCubies()[idx].colors[static_cast<int>(Face::DOWN)] == Colors::YELLOW) {
                break;
            }
            doAlg(cube, "R' U' R U");
        }
        doMove(cube, Face::DOWN, Direction::CW);
    }
}

void Solver3x3::permuteYellowCorners(RubiksCube& cube) {
    auto getCornerColor = [&](int x, int y, int z, Face f) {
        for (const auto& c : cube.getCubies()) {
            if (c.ix == x && c.iy == y && c.iz == z) {
                return c.colors[static_cast<int>(f)];
            }
        }
        return Colors::NONE;
    };

    auto getHeadlightsFace = [&]() {
        bool fMatch = (getCornerColor(0, 0, 2, Face::FRONT) == getCornerColor(2, 0, 2, Face::FRONT));
        bool rMatch = (getCornerColor(2, 0, 2, Face::RIGHT) == getCornerColor(2, 0, 0, Face::RIGHT));
        bool bMatch = (getCornerColor(2, 0, 0, Face::BACK) == getCornerColor(0, 0, 0, Face::BACK));
        bool lMatch = (getCornerColor(0, 0, 0, Face::LEFT) == getCornerColor(0, 0, 2, Face::LEFT));

        int matchCount = (fMatch ? 1 : 0) + (rMatch ? 1 : 0) + (bMatch ? 1 : 0) + (lMatch ? 1 : 0);
        if (matchCount == 4) return Face::DOWN;
        if (fMatch) return Face::FRONT;
        if (rMatch) return Face::RIGHT;
        if (bMatch) return Face::BACK;
        if (lMatch) return Face::LEFT;
        return Face::UP;
    };

    while (true) {
        Face headlights = getHeadlightsFace();
        if (headlights == Face::DOWN) break;

        if (headlights == Face::UP) {
            doAlg(cube, "R' F R' B2 R F' R' B2 R2");
        } else {
            if (headlights == Face::BACK) {
                doAlg(cube, "R' F R' B2 R F' R' B2 R2");
            } else if (headlights == Face::LEFT) {
                doAlg(cube, "D' R' F R' B2 R F' R' B2 R2");
            } else if (headlights == Face::FRONT) {
                doAlg(cube, "D2 R' F R' B2 R F' R' B2 R2");
            } else if (headlights == Face::RIGHT) {
                doAlg(cube, "D R' F R' B2 R F' R' B2 R2");
            }
        }
    }

    // Align corners with side centers
    while (true) {
        Colors::ColorName rColor = Colors::NONE;
        for (const auto& c : cube.getCubies()) {
            if (c.ix == 2 && c.iy == 0 && c.iz == 2) {
                rColor = c.colors[static_cast<int>(Face::RIGHT)];
                break;
            }
        }
        if (rColor == Colors::RED) break;
        doMove(cube, Face::DOWN, Direction::CW);
    }
}

void Solver3x3::permuteYellowEdges(RubiksCube& cube) {
    auto getEdgeColor = [&](int x, int y, int z, Face f) {
        for (const auto& c : cube.getCubies()) {
            if (c.ix == x && c.iy == y && c.iz == z) {
                return c.colors[static_cast<int>(f)];
            }
        }
        return Colors::NONE;
    };

    while (true) {
        bool fCorrect = (getEdgeColor(1, 0, 2, Face::FRONT) == Colors::GREEN);
        bool rCorrect = (getEdgeColor(2, 0, 1, Face::RIGHT) == Colors::RED);
        bool bCorrect = (getEdgeColor(1, 0, 0, Face::BACK) == Colors::BLUE);
        bool lCorrect = (getEdgeColor(0, 0, 1, Face::LEFT) == Colors::ORANGE);

        int correctCount = (fCorrect ? 1 : 0) + (rCorrect ? 1 : 0) + (bCorrect ? 1 : 0) + (lCorrect ? 1 : 0);
        if (correctCount == 4) break;

        if (correctCount == 0) {
            applyUPerf(cube, Face::BACK, true);
        } else if (correctCount == 1) {
            Face correctFace = Face::BACK;
            if (fCorrect) correctFace = Face::FRONT;
            if (rCorrect) correctFace = Face::RIGHT;
            if (bCorrect) correctFace = Face::BACK;
            if (lCorrect) correctFace = Face::LEFT;

            applyUPerf(cube, correctFace, true);
        }
    }
}

std::vector<Move> Solver3x3::solve(const RubiksCube& cube) {
    solutionMoves.clear();
    RubiksCube copyCube = cube;
    
    std::cout << "  - Starting White Cross..." << std::endl;
    solveWhiteCross(copyCube);
    std::cout << "  - White Cross Done. Starting White Corners..." << std::endl;
    solveWhiteCorners(copyCube);
    std::cout << "  - White Corners Done. Starting Middle Layer..." << std::endl;
    solveMiddleLayer(copyCube);
    std::cout << "  - Middle Layer Done. Starting Yellow Cross..." << std::endl;
    solveYellowCross(copyCube);
    std::cout << "  - Yellow Cross Done. Starting Yellow Corners Orientation..." << std::endl;
    orientYellowCorners(copyCube);
    std::cout << "  - Yellow Corners Orientation Done. Starting Yellow Corners Permutation..." << std::endl;
    permuteYellowCorners(copyCube);
    std::cout << "  - Yellow Corners Permutation Done. Starting Yellow Edges Permutation..." << std::endl;
    permuteYellowEdges(copyCube);
    std::cout << "  - Yellow Edges Permutation Done. Cube Solved!" << std::endl;
    
    return solutionMoves;
}
