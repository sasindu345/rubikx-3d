#pragma once

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>
#include "ui/SolutionPlayer.h"

class HUD {
private:
    void drawPanel(int x, int y, int w, int h, float r, float g, float b, float a);
    void drawBorder(int x, int y, int w, int h, float r, float g, float b, float a, float lineWidth = 1.0f);
    void drawString(int x, int y, const std::string& str, void* font, float r, float g, float b);
    void drawControlLine(int x, int y, const std::string& key, const std::string& desc, void* font);

public:
    HUD();
    ~HUD();

    void render(int width, int height, const SolutionPlayer& player, bool showHelp);
};
