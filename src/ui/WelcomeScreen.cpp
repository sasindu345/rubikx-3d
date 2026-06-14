/*
--------------------------------------------------
Module: Welcome Screen System

Purpose:
Renders the initial welcome screen configuration board, allowing interactive
selection of cube parameters (dimension size, style, colors, and preferences) before puzzle initialization.

Graphics Concepts:
- 2D Orthographic Projections
- Disabling Depth Buffer / Lighting States
- Option Cards and Focus Borders
- Raster Font Text Alignment

Mathematics:
- Discretized View Coordinates
- Horizontal/Vertical alignment geometry math
- Selection focus grid coordinate indexing

Responsibilities:
- Rendering option boxes and highlighted focus frames
- Setting up orthographic menu projections
- Checking mouse clicks against button bounds (interactive click zones)
- Handling keyboard control sequences for menu selection
--------------------------------------------------
*/

#include "WelcomeScreen.h"
#include "algorithms/Algorithms.h"
#include <cstdio>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

WelcomeScreen::WelcomeScreen() {
    active = true;
    config.cubeSize = 3;
    config.renderMode = RenderMode::SOLID;
    config.colorScheme = Colors::Scheme::CLASSIC;
    config.glassMode = false;
    config.practiceMode = false;
    config.autoScramble = true;

    activeRow = 0;
    activeCol = 1; // 3x3 is default
}

WelcomeScreen::~WelcomeScreen() {}

void WelcomeScreen::drawPanel(int x, int y, int w, int h, float r, float g, float b, float a) {
    rasterizeQuad(x, y, x + w, y, x + w, y + h, x, y + h, r, g, b, a);
}

void WelcomeScreen::drawBorder(int x, int y, int w, int h, float r, float g, float b, float a, float lineWidth) {
    rasterizeBorder(x, y, x + w, y, x + w, y + h, x, y + h, r, g, b, a, lineWidth);
}

void WelcomeScreen::drawString(int x, int y, const std::string& str, void* font, float r, float g, float b) {
    rasterizeFont(x, y, str, font, r, g, b);
}

void WelcomeScreen::drawOptionCard(int x, int y, int w, int h, const std::string& label, bool selected, bool focused, float r, float g, float b) {
    if (selected) {
        // Muted accent background (40% brightness) so white text reads clearly
        drawPanel(x, y, w, h, r * 0.4f, g * 0.4f, b * 0.4f, 1.0f);
        // Bright accent border to indicate selection
        drawBorder(x, y, w, h, r, g, b, 1.0f, 1.5f);
    } else {
        // Dark neutral background for unselected card
        drawPanel(x, y, w, h, 0.14f, 0.14f, 0.18f, 1.0f);
        drawBorder(x, y, w, h, 0.25f, 0.25f, 0.30f, 0.8f, 1.0f);
    }

    if (focused) {
        // Bright orange keyboard focus indicator ring
        drawBorder(x - 2, y - 2, w + 4, h + 4, 1.0f, 0.65f, 0.0f, 1.0f, 2.0f);
    }

    // Center the text inside the card
    int textW = glutBitmapLength(GLUT_BITMAP_8_BY_13, reinterpret_cast<const unsigned char*>(label.c_str()));
    int textX = x + (w - textW) / 2;
    int textY = y + (h + 9) / 2; // center 13px font vertically

    if (selected) {
        // Bright white text on dark accent background
        drawString(textX, textY, label, GLUT_BITMAP_8_BY_13, 1.0f, 1.0f, 1.0f);
    } else {
        // Light grey text on dark background
        drawString(textX, textY, label, GLUT_BITMAP_8_BY_13, 0.75f, 0.75f, 0.8f);
    }
}

void WelcomeScreen::render(int width, int height) {
    setupOrthographicProjection(width, height);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ========== FULL-SCREEN SOLID DARK BACKGROUND ==========
    // No 3D cube is rendered behind this — clean dark canvas
    drawPanel(0, 0, width, height, 0.07f, 0.07f, 0.09f, 1.0f);

    // Subtle decorative top accent line across full width
    drawPanel(0, 0, width, 3, 0.0f, 0.6f, 0.9f, 0.6f);

    // ========== CENTERED MODAL CARD ==========
    int modalW = 620;
    int modalH = 580;
    int modalX = (width - modalW) / 2;
    int modalY = (height - modalH) / 2;

    // Modal background with subtle raised look
    drawPanel(modalX, modalY, modalW, modalH, 0.10f, 0.10f, 0.13f, 1.0f);
    // Outer glow border
    drawBorder(modalX, modalY, modalW, modalH, 0.0f, 0.55f, 0.8f, 0.5f, 1.5f);
    // Inner subtle border
    drawBorder(modalX + 4, modalY + 4, modalW - 8, modalH - 8, 0.18f, 0.18f, 0.22f, 0.6f, 1.0f);

    // ========== HEADER ==========
    // Title text
    std::string titleText = "RUBIKX-3D";
    int titleW = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(titleText.c_str()));
    drawString(modalX + (modalW - titleW) / 2, modalY + 38, titleText, GLUT_BITMAP_HELVETICA_18, 0.0f, 0.85f, 1.0f);
    // Subtitle
    std::string subtitleText = "Configure your puzzle before you begin";
    int subtitleW = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, reinterpret_cast<const unsigned char*>(subtitleText.c_str()));
    drawString(modalX + (modalW - subtitleW) / 2, modalY + 58, subtitleText, GLUT_BITMAP_HELVETICA_12, 0.55f, 0.55f, 0.6f);

    // Header separator line
    drawPanel(modalX + 30, modalY + 72, modalW - 60, 1, 0.2f, 0.2f, 0.25f, 0.8f);

    // ========== BUILD INTERACTIVE ZONES ==========
    zones.clear();

    int startY = modalY + 88;
    int cardH = 34;
    int startX = modalX + 30;
    int contentW = modalW - 60; // usable width inside modal

    // ──────────────────────────────────────────────
    // ROW 0: CUBE SIZE
    // ──────────────────────────────────────────────
    drawPanel(startX, startY + 8, 4, 14, 0.0f, 0.8f, 1.0f, 1.0f); // Cyan accent dot
    drawString(startX + 12, startY + 18, "CUBE SIZE", GLUT_BITMAP_HELVETICA_12, 0.9f, 0.9f, 0.95f);
    drawString(startX + 120, startY + 18, "Select puzzle dimension", GLUT_BITMAP_HELVETICA_10, 0.45f, 0.45f, 0.5f);

    int sizes[] = {2, 3, 4, 5, 6, 7};
    int sizeCount = 6;
    int sizeW = (contentW - (sizeCount - 1) * 10) / sizeCount; // evenly distribute
    for (int i = 0; i < sizeCount; ++i) {
        int xPos = startX + i * (sizeW + 10);
        bool selected = (config.cubeSize == sizes[i]);
        bool focused = (activeRow == 0 && activeCol == i);
        char lbl[16];
        snprintf(lbl, sizeof(lbl), "%dx%d", sizes[i], sizes[i]);

        drawOptionCard(xPos, startY + 30, sizeW, cardH, lbl, selected, focused, 0.0f, 0.8f, 1.0f);

        zones.push_back({xPos, startY + 30, xPos + sizeW, startY + 30 + cardH, 0, i, lbl, sizes[i]});
    }

    // Row separator
    drawPanel(startX, startY + 74, contentW, 1, 0.18f, 0.18f, 0.22f, 0.6f);

    // ──────────────────────────────────────────────
    // ROW 1: RENDER STYLE
    // ──────────────────────────────────────────────
    startY += 84;
    drawPanel(startX, startY + 8, 4, 14, 0.9f, 0.35f, 0.35f, 1.0f); // Red accent dot
    drawString(startX + 12, startY + 18, "RENDER STYLE", GLUT_BITMAP_HELVETICA_12, 0.9f, 0.9f, 0.95f);
    drawString(startX + 130, startY + 18, "Visual rendering mode", GLUT_BITMAP_HELVETICA_10, 0.45f, 0.45f, 0.5f);

    std::string renders[] = {"Solid", "Wireframe", "Textured"};
    RenderMode renderValues[] = {RenderMode::SOLID, RenderMode::WIREFRAME, RenderMode::TEXTURED};
    int renderW = (contentW - 2 * 10) / 3;
    for (int i = 0; i < 3; ++i) {
        int xPos = startX + i * (renderW + 10);
        bool selected = (config.renderMode == renderValues[i]);
        bool focused = (activeRow == 1 && activeCol == i);

        drawOptionCard(xPos, startY + 30, renderW, cardH, renders[i], selected, focused, 0.9f, 0.35f, 0.35f);

        zones.push_back({xPos, startY + 30, xPos + renderW, startY + 30 + cardH, 1, i, renders[i], (int)renderValues[i]});
    }

    // Row separator
    drawPanel(startX, startY + 74, contentW, 1, 0.18f, 0.18f, 0.22f, 0.6f);

    // ──────────────────────────────────────────────
    // ROW 2: COLOR SCHEME
    // ──────────────────────────────────────────────
    startY += 84;
    drawPanel(startX, startY + 8, 4, 14, 0.0f, 0.85f, 0.4f, 1.0f); // Green accent dot
    drawString(startX + 12, startY + 18, "COLOR SCHEME", GLUT_BITMAP_HELVETICA_12, 0.9f, 0.9f, 0.95f);
    drawString(startX + 130, startY + 18, "Sticker color palette", GLUT_BITMAP_HELVETICA_10, 0.45f, 0.45f, 0.5f);

    std::string schemes[] = {"Classic", "Pastel", "High Contrast"};
    Colors::Scheme schemeValues[] = {Colors::Scheme::CLASSIC, Colors::Scheme::PASTEL, Colors::Scheme::HIGH_CONTRAST};
    int schemeW = (contentW - 2 * 10) / 3;
    for (int i = 0; i < 3; ++i) {
        int xPos = startX + i * (schemeW + 10);
        bool selected = (config.colorScheme == schemeValues[i]);
        bool focused = (activeRow == 2 && activeCol == i);

        drawOptionCard(xPos, startY + 30, schemeW, cardH, schemes[i], selected, focused, 0.0f, 0.85f, 0.4f);

        zones.push_back({xPos, startY + 30, xPos + schemeW, startY + 30 + cardH, 2, i, schemes[i], (int)schemeValues[i]});
    }

    // Row separator
    drawPanel(startX, startY + 74, contentW, 1, 0.18f, 0.18f, 0.22f, 0.6f);

    // ──────────────────────────────────────────────
    // ROW 3: PREFERENCES
    // ──────────────────────────────────────────────
    startY += 84;
    drawPanel(startX, startY + 8, 4, 14, 0.7f, 0.45f, 0.95f, 1.0f); // Purple accent dot
    drawString(startX + 12, startY + 18, "PREFERENCES", GLUT_BITMAP_HELVETICA_12, 0.9f, 0.9f, 0.95f);
    drawString(startX + 130, startY + 18, "Toggle game options", GLUT_BITMAP_HELVETICA_10, 0.45f, 0.45f, 0.5f);

    std::string optLabels[] = {
        config.glassMode    ? "[X] Glass Cube"  : "[ ] Glass Cube",
        config.practiceMode ? "[X] Practice"    : "[ ] Practice",
        config.autoScramble ? "[X] Auto-Scramble": "[ ] Auto-Scramble"
    };
    int optW = (contentW - 2 * 10) / 3;
    for (int i = 0; i < 3; ++i) {
        int xPos = startX + i * (optW + 10);
        bool selected = false;
        if (i == 0) selected = config.glassMode;
        else if (i == 1) selected = config.practiceMode;
        else if (i == 2) selected = config.autoScramble;

        bool focused = (activeRow == 3 && activeCol == i);

        drawOptionCard(xPos, startY + 30, optW, cardH, optLabels[i], selected, focused, 0.7f, 0.45f, 0.95f);

        zones.push_back({xPos, startY + 30, xPos + optW, startY + 30 + cardH, 3, i, optLabels[i], selected ? 1 : 0});
    }

    // ──────────────────────────────────────────────
    // ROW 4: START GAME BUTTON
    // ──────────────────────────────────────────────
    startY += 84;
    int btnW = 340;
    int btnH = 48;
    int btnX = modalX + (modalW - btnW) / 2;
    int btnY = startY + 8;
    bool btnFocused = (activeRow == 4);

    if (btnFocused) {
        // Bright orange focused state
        drawPanel(btnX, btnY, btnW, btnH, 1.0f, 0.55f, 0.0f, 1.0f);
        drawBorder(btnX, btnY, btnW, btnH, 1.0f, 0.85f, 0.2f, 1.0f, 2.5f);

        std::string txt = ">> START GAME <<";
        int tw = glutBitmapLength(GLUT_BITMAP_9_BY_15, reinterpret_cast<const unsigned char*>(txt.c_str()));
        drawString(btnX + (btnW - tw) / 2, btnY + 30, txt, GLUT_BITMAP_9_BY_15, 0.0f, 0.0f, 0.0f);
    } else {
        // Default teal state
        drawPanel(btnX, btnY, btnW, btnH, 0.0f, 0.50f, 0.70f, 1.0f);
        drawBorder(btnX, btnY, btnW, btnH, 0.0f, 0.7f, 0.95f, 0.9f, 1.5f);

        std::string txt = "START GAME";
        int tw = glutBitmapLength(GLUT_BITMAP_9_BY_15, reinterpret_cast<const unsigned char*>(txt.c_str()));
        drawString(btnX + (btnW - tw) / 2, btnY + 30, txt, GLUT_BITMAP_9_BY_15, 1.0f, 1.0f, 1.0f);
    }

    zones.push_back({btnX, btnY, btnX + btnW, btnY + btnH, 4, 0, "START", 0});

    // ========== FOOTER HELP TEXT ==========
    drawPanel(modalX + 30, modalY + modalH - 35, contentW, 1, 0.18f, 0.18f, 0.22f, 0.4f);
    std::string footerText = "ARROWS navigate  |  SPACE select  |  ENTER start  |  or click with MOUSE";
    int footerW = glutBitmapLength(GLUT_BITMAP_HELVETICA_10, reinterpret_cast<const unsigned char*>(footerText.c_str()));
    drawString(modalX + (modalW - footerW) / 2, modalY + modalH - 18, footerText, GLUT_BITMAP_HELVETICA_10, 0.4f, 0.4f, 0.45f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);

    restoreProjection();
}

bool WelcomeScreen::handleKey(unsigned char key, int x, int y, bool& outStartGame) {
    outStartGame = false;
    if (!active) return false;

    if (key == 13 || key == 10) { // Enter key
        if (activeRow == 4) {
            // Clicked start button
            active = false;
            outStartGame = true;
            return true;
        } else {
            // Jump directly to start button or select active option
            activeRow = 4;
            activeCol = 0;
            return true;
        }
    } else if (key == ' ') { // Space key to select/toggle
        if (activeRow == 0) { // size
            int sizes[] = {2, 3, 4, 5, 6, 7};
            config.cubeSize = sizes[activeCol];
        } else if (activeRow == 1) { // render mode
            RenderMode renderValues[] = {RenderMode::SOLID, RenderMode::WIREFRAME, RenderMode::TEXTURED};
            config.renderMode = renderValues[activeCol];
        } else if (activeRow == 2) { // color scheme
            Colors::Scheme schemeValues[] = {Colors::Scheme::CLASSIC, Colors::Scheme::PASTEL, Colors::Scheme::HIGH_CONTRAST};
            config.colorScheme = schemeValues[activeCol];
        } else if (activeRow == 3) { // preferences
            if (activeCol == 0) config.glassMode = !config.glassMode;
            else if (activeCol == 1) config.practiceMode = !config.practiceMode;
            else if (activeCol == 2) config.autoScramble = !config.autoScramble;
        } else if (activeRow == 4) {
            active = false;
            outStartGame = true;
        }
        return true;
    } else if (key == 27) { // Escape - just close it if config is set
        active = false;
        outStartGame = true;
        return true;
    }

    return false;
}

void WelcomeScreen::handleSpecialKey(int key, int x, int y) {
    if (!active) return;

    switch (key) {
        case GLUT_KEY_UP:
            activeRow = (activeRow - 1 + 5) % 5;
            // Bound activeCol to maximum valid column for the new row
            if (activeRow == 0) { // 6 sizes
                if (activeCol >= 6) activeCol = 5;
            } else if (activeRow == 1 || activeRow == 2 || activeRow == 3) { // 3 options
                if (activeCol >= 3) activeCol = 2;
            } else if (activeRow == 4) { // 1 button
                activeCol = 0;
            }
            break;
        case GLUT_KEY_DOWN:
            activeRow = (activeRow + 1) % 5;
            if (activeRow == 0) {
                if (activeCol >= 6) activeCol = 5;
            } else if (activeRow == 1 || activeRow == 2 || activeRow == 3) {
                if (activeCol >= 3) activeCol = 2;
            } else if (activeRow == 4) {
                activeCol = 0;
            }
            break;
        case GLUT_KEY_LEFT:
            if (activeRow == 0) {
                activeCol = (activeCol - 1 + 6) % 6;
            } else if (activeRow == 1 || activeRow == 2 || activeRow == 3) {
                activeCol = (activeCol - 1 + 3) % 3;
            }
            break;
        case GLUT_KEY_RIGHT:
            if (activeRow == 0) {
                activeCol = (activeCol + 1) % 6;
            } else if (activeRow == 1 || activeRow == 2 || activeRow == 3) {
                activeCol = (activeCol + 1) % 3;
            }
            break;
    }
}

bool WelcomeScreen::handleMouse(int button, int state, int x, int y, bool& outStartGame) {
    outStartGame = false;
    if (!active) return false;

    // We only process mouse clicks on GLUT_DOWN
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        for (const auto& zone : zones) {
            if (x >= zone.x1 && x <= zone.x2 && y >= zone.y1 && y <= zone.y2) {
                // Click is within button zone!
                activeRow = zone.row;
                activeCol = zone.col;

                if (zone.row == 0) {
                    config.cubeSize = zone.value;
                } else if (zone.row == 1) {
                    config.renderMode = (RenderMode)zone.value;
                } else if (zone.row == 2) {
                    config.colorScheme = (Colors::Scheme)zone.value;
                } else if (zone.row == 3) {
                    if (zone.col == 0) config.glassMode = !config.glassMode;
                    else if (zone.col == 1) config.practiceMode = !config.practiceMode;
                    else if (zone.col == 2) config.autoScramble = !config.autoScramble;
                } else if (zone.row == 4) {
                    active = false;
                    outStartGame = true;
                }
                return true; // Click handled
            }
        }
    }
    return false;
}
