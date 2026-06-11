#pragma once
#include "solver/Solver.h"
#include <string>

class Solver3x3 : public Solver {
public:
    std::vector<Move> solutionMoves;

    void doMove(RubiksCube& cube, Face face, Direction dir, int layer = 0);
    void doMove(RubiksCube& cube, const Move& m);
    void doAlg(RubiksCube& cube, const std::string& alg);

    // Helpers to locate pieces by color combinations
    int findEdge(const RubiksCube& cube, Colors::ColorName c1, Colors::ColorName c2) const;
    int findCorner(const RubiksCube& cube, Colors::ColorName c1, Colors::ColorName c2, Colors::ColorName c3) const;

    // Helper face mappings
    Face getFaceFromColor(Colors::ColorName color) const;
    bool isRightOf(Face fSide, Face fBot) const;
    std::string getInsertionAlg(Face fSide, bool toRight) const;
    Face getRightFaceOfBack(Face fBack) const;
    void applyUPerf(RubiksCube& cube, Face fBack, bool cw);

    // Beginner method stages
    void solveWhiteCross(RubiksCube& cube);
    void solveWhiteCorners(RubiksCube& cube);
    void solveMiddleLayer(RubiksCube& cube);
    void solveYellowCross(RubiksCube& cube);
    void orientYellowCorners(RubiksCube& cube);
    void permuteYellowCorners(RubiksCube& cube);
    void permuteYellowEdges(RubiksCube& cube);

public:
    Solver3x3();
    ~Solver3x3() override;
    std::vector<Move> solve(const RubiksCube& cube) override;
};
