#pragma once
#include <string>
#include "SolutionPlayer.h"
#include "../utils/ScoreManager.h"

class HUD {
private:
    void drawPanel(int x, int y, int w, int h, float r, float g, float b, float a);
    void drawBorder(int x, int y, int w, int h, float r, float g, float b, float a, float lineWidth);
    void drawString(int x, int y, const std::string& str, void* font, float r, float g, float b);
    void drawControlLine(int x, int y, const std::string& key, const std::string& desc, void* font);

public:
    HUD();
    ~HUD();

    // Renders the 2D overlays (controls menu, playback dashboards, status hints)
    // alphaBlending: current glass cube mode state (for status display)
    // renderMode: current rendering mode (0 = Solid, 1 = Wireframe, 2 = Textured)
    void render(int width, int height, const SolutionPlayer& player, bool showHelp, bool alphaBlending = false, int renderMode = 0, int cubeSize = 3);

    // Renders the scoring panel (top-right, below the Glass Cube badge):
    // live move counter + stopwatch while solving, and the score/best
    // score once a solve has been completed.
    void renderScorePanel(int width, int height, const ScoreManager& scoreManager, int cubeSize, bool practiceMode);

    // Renders the stats panel showing Ao5, Ao12, and totals
    void renderStatsPanel(int width, int height, const ScoreManager& scoreManager, int cubeSize);
};
