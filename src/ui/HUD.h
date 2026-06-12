#pragma once
#include <string>
#include "SolutionPlayer.h"

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
    void render(int width, int height, const SolutionPlayer& player, bool showHelp, bool alphaBlending = false);
};
