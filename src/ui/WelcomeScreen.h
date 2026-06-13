#pragma once
#include <string>
#include <vector>
#include "cube/RubiksCube.h"
#include "Renderer.h"
#include "Colors.h"

struct WelcomeConfig {
    int cubeSize = 3;
    RenderMode renderMode = RenderMode::SOLID;
    Colors::Scheme colorScheme = Colors::Scheme::CLASSIC;
    bool glassMode = false;
    bool practiceMode = false;
    bool autoScramble = true;
};

class WelcomeScreen {
public:
    WelcomeScreen();
    ~WelcomeScreen();

    // Renders the full screen overlay
    void render(int width, int height);

    // Processes standard keyboard keys
    // Returns true if state changed or we need to start
    bool handleKey(unsigned char key, int x, int y, bool& outStartGame);

    // Processes special keys (arrows)
    void handleSpecialKey(int key, int x, int y);

    // Processes mouse clicks
    // Returns true if a button was clicked
    bool handleMouse(int button, int state, int x, int y, bool& outStartGame);

    bool isActive() const { return active; }
    void setActive(bool a) { active = a; }

    const WelcomeConfig& getConfig() const { return config; }
    void setConfig(const WelcomeConfig& cfg) { config = cfg; }

private:
    bool active = true;
    WelcomeConfig config;

    // Navigation state for keyboard focus
    // 0: Cube Size, 1: Render Mode, 2: Color Scheme, 3: Options (Glass, Practice, Auto-Scramble), 4: START button
    int activeRow = 0;
    int activeCol = 0; // selected option within the row

    // Layout structures for mouse interaction
    struct RectZone {
        int x1, y1, x2, y2;
        int row;
        int col;
        std::string label;
        int value; // option value
    };
    std::vector<RectZone> zones;

    void drawPanel(int x, int y, int w, int h, float r, float g, float b, float a);
    void drawBorder(int x, int y, int w, int h, float r, float g, float b, float a, float lineWidth);
    void drawString(int x, int y, const std::string& str, void* font, float r, float g, float b);
    
    // Draw option selector card
    void drawOptionCard(int x, int y, int w, int h, const std::string& label, bool selected, bool focused, float r, float g, float b);
};
