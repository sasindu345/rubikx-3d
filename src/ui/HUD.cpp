#include "HUD.h"
#include "algorithms/Algorithms.h"
#include <cstdio>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

HUD::HUD() {}
HUD::~HUD() {}

void HUD::drawPanel(int x, int y, int w, int h, float r, float g, float b, float a) {
    rasterizeQuad(x, y, x + w, y, x + w, y + h, x, y + h, r, g, b, a);
}

void HUD::drawBorder(int x, int y, int w, int h, float r, float g, float b, float a, float lineWidth) {
    rasterizeBorder(x, y, x + w, y, x + w, y + h, x, y + h, r, g, b, a, lineWidth);
}

void HUD::drawString(int x, int y, const std::string& str, void* font, float r, float g, float b) {
    rasterizeFont(x, y, str, font, r, g, b);
}

void HUD::drawControlLine(int x, int y, const std::string& key, const std::string& desc, void* font) {
    // Key in electric cyan
    drawString(x, y, key, font, 0.0f, 0.8f, 1.0f);
    // Description in off-white
    drawString(x + 75, y, desc, font, 0.85f, 0.85f, 0.9f);
}

void HUD::render(int width, int height, const SolutionPlayer& player, bool showHelp, bool alphaBlending) {
    // 2D viewing transformation
    setupOrthographicProjection(width, height);

    // Disable depth testing and lighting for 2D rendering
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 1. Draw left help panel if visible
    if (showHelp) {
        int panelW = 310;
        // Draw main dark panel
        drawPanel(0, 0, panelW, height, 0.08f, 0.08f, 0.1f, 0.85f);
        // Draw glowing right border
        drawPanel(panelW - 2, 0, 2, height, 0.2f, 0.4f, 0.6f, 0.6f);

        int startY = 35;
        int lineSpacing = 20;

        // Title
        drawString(20, startY, "RUBIKX-3D SYSTEM", GLUT_BITMAP_HELVETICA_18, 1.0f, 0.6f, 0.1f);
        startY += 18;
        drawString(20, startY, "Interactive Learning & Solving", GLUT_BITMAP_HELVETICA_10, 0.6f, 0.6f, 0.7f);
        startY += 30;

        // --- SYSTEM CONTROLS ---
        drawString(20, startY, "SYSTEM CONTROLS", GLUT_BITMAP_HELVETICA_12, 1.0f, 0.75f, 0.2f);
        drawPanel(20, startY + 4, 120, 1, 1.0f, 0.75f, 0.2f, 0.5f);
        startY += 20;
        drawControlLine(20, startY, "S / s", "Scramble puzzle (20 turns)", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "Z / z", "Solve cube (History Reversal)", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "C / c", "Reset cube to solved state", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "Space", "Pause/Resume timer", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "M / m", "Toggle practice mode", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "H / h", "Toggle this help menu", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "2", "Switch to 2x2 Rubik's Cube", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "3", "Switch to 3x3 Rubik's Cube", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "4", "Switch to 4x4 Rubik's Cube", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "5", "Switch to 5x5 Rubik's Cube", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "6", "Switch to 6x6 Rubik's Cube", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "7", "Switch to 7x7 Rubik's Cube", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "Esc", "Exit application", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "T / t", "Toggle Glass Cube (Alpha Blend)", GLUT_BITMAP_8_BY_13); startY += 35;

        // --- SOLUTION PLAYBACK ---
        drawString(20, startY, "PLAYBACK CONTROLS", GLUT_BITMAP_HELVETICA_12, 1.0f, 0.75f, 0.2f);
        drawPanel(20, startY + 4, 135, 1, 1.0f, 0.75f, 0.2f, 0.5f);
        startY += 20;
        drawControlLine(20, startY, "P / p", "Play / Pause autoplay", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, ", / <", "Step backward (Undo move)", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, ". / >", "Step forward (Play move)", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "- / +", "Adjust playback speed", GLUT_BITMAP_8_BY_13); startY += 35;

        // --- CAMERA CONTROLS ---
        drawString(20, startY, "CAMERA VIEW", GLUT_BITMAP_HELVETICA_12, 1.0f, 0.75f, 0.2f);
        drawPanel(20, startY + 4, 90, 1, 1.0f, 0.75f, 0.2f, 0.5f);
        startY += 20;
        drawString(20, startY, "Left Click + Drag: Orbit view", GLUT_BITMAP_HELVETICA_12, 0.85f, 0.85f, 0.9f); startY += lineSpacing;
        drawString(20, startY, "Right Click + Drag: Zoom view", GLUT_BITMAP_HELVETICA_12, 0.85f, 0.85f, 0.9f); startY += lineSpacing;
        drawString(20, startY, "Scroll Wheel: Zoom in/out", GLUT_BITMAP_HELVETICA_12, 0.85f, 0.85f, 0.9f); startY += lineSpacing;
        drawString(20, startY, "Arrow Keys: Orbit view step", GLUT_BITMAP_HELVETICA_12, 0.85f, 0.85f, 0.9f); startY += 35;

        // --- MANUAL ROTATIONS ---
        drawString(20, startY, "MANUAL TURNS (CW / CCW)", GLUT_BITMAP_HELVETICA_12, 1.0f, 0.75f, 0.2f);
        drawPanel(20, startY + 4, 160, 1, 1.0f, 0.75f, 0.2f, 0.5f);
        startY += 20;
        drawControlLine(20, startY, "R / r", "Right face rotation", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "L / l", "Left face rotation", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "U / u", "Up (Top) face rotation", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "D / d", "Down (Bottom) rotation", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "F / f", "Front face rotation", GLUT_BITMAP_8_BY_13); startY += lineSpacing;
        drawControlLine(20, startY, "B / b", "Back face rotation", GLUT_BITMAP_8_BY_13);
    } else {
        // Simple minimized HUD banner at top-left
        drawPanel(10, 10, 190, 30, 0.08f, 0.08f, 0.1f, 0.75f);
        drawBorder(10, 10, 190, 30, 0.2f, 0.4f, 0.6f, 0.5f, 1.0f);
        drawString(20, 30, "Press 'H' for Help Menu", GLUT_BITMAP_HELVETICA_12, 0.0f, 0.8f, 1.0f);
    }

    // Glass Cube status badge (top-right corner, always visible)
    {
        int badgeW = 200;
        int badgeX = width - badgeW - 10;
        int badgeY = 10;
        int badgeH = 28;
        if (alphaBlending) {
            // Glowing teal badge when ON
            drawPanel(badgeX, badgeY, badgeW, badgeH, 0.0f, 0.35f, 0.45f, 0.85f);
            drawBorder(badgeX, badgeY, badgeW, badgeH, 0.0f, 0.8f, 1.0f, 0.9f, 1.5f);
            drawString(badgeX + 12, badgeY + 19, "GLASS CUBE  [T] ON", GLUT_BITMAP_HELVETICA_12, 0.0f, 1.0f, 0.9f);
        } else {
            // Dimmed badge when OFF
            drawPanel(badgeX, badgeY, badgeW, badgeH, 0.08f, 0.08f, 0.1f, 0.65f);
            drawBorder(badgeX, badgeY, badgeW, badgeH, 0.2f, 0.2f, 0.25f, 0.5f, 1.0f);
            drawString(badgeX + 12, badgeY + 19, "GLASS CUBE  [T] OFF", GLUT_BITMAP_HELVETICA_12, 0.4f, 0.4f, 0.45f);
        }
    }

    // 2. Playback Dashboard (Bottom Center)
    if (player.getMoveCount() > 0) {
        int startX = showHelp ? 330 : 30;
        int endX = width - 30;
        int dashW = 680;
        
        // Center the dashboard in the available space if space is larger than dashW
        int availableSpace = endX - startX;
        int dashX = startX + (availableSpace - dashW) / 2;
        if (dashX < startX) {
            dashX = startX;
            dashW = availableSpace;
        }
        
        int dashH = 95;
        int dashY = height - dashH - 20;

        // Draw background panel and border
        drawPanel(dashX, dashY, dashW, dashH, 0.08f, 0.08f, 0.1f, 0.9f);
        drawBorder(dashX, dashY, dashW, dashH, 0.2f, 0.4f, 0.6f, 0.6f, 1.5f);

        // Status Indicators
        std::string statusStr = "STATUS: ";
        float statusR = 0.6f, statusG = 0.6f, statusB = 0.6f;
        if (player.isPlaying()) {
            statusStr += "PLAYING";
            statusR = 0.0f; statusG = 0.85f; statusB = 0.4f; // Teal/green
        } else if (player.getCurrentIndex() >= player.getMoveCount()) {
            statusStr += "COMPLETED";
            statusR = 0.0f; statusG = 0.8f; statusB = 1.0f; // Electric cyan
        } else {
            statusStr += "PAUSED";
            statusR = 1.0f; statusG = 0.55f; statusB = 0.0f; // Orange
        }

        drawString(dashX + 25, dashY + 30, statusStr, GLUT_BITMAP_HELVETICA_12, statusR, statusG, statusB);

        // Progress counter
        std::string stepStr = "Step: " + std::to_string(player.getCurrentIndex()) + " / " + std::to_string(player.getMoveCount());
        drawString(dashX + 220, dashY + 30, stepStr, GLUT_BITMAP_HELVETICA_12, 0.9f, 0.9f, 0.95f);

        // Speed indicator
        char speedBuf[32];
        snprintf(speedBuf, sizeof(speedBuf), "Speed: %.1f moves/s", player.getSpeed());
        drawString(dashX + 380, dashY + 30, speedBuf, GLUT_BITMAP_HELVETICA_12, 0.9f, 0.9f, 0.95f);

        // Playback hints
        drawString(dashX + 540, dashY + 30, "[P] Play  [,] Prev  [.] Next", GLUT_BITMAP_HELVETICA_10, 0.6f, 0.6f, 0.7f);

        // Draw recessed tape container
        int tapeX = dashX + 20;
        int tapeY = dashY + 45;
        int tapeW = dashW - 40;
        int tapeH = 36;
        drawPanel(tapeX, tapeY, tapeW, tapeH, 0.04f, 0.04f, 0.06f, 0.95f);
        drawBorder(tapeX, tapeY, tapeW, tapeH, 0.15f, 0.15f, 0.2f, 0.8f, 1.0f);

        // Draw move tape
        const auto& moves = player.getMoves();
        int currIdx = player.getCurrentIndex();
        int centerX = tapeX + tapeW / 2;
        int slotW = 55; // width per move token

        // Calculate how many slots fit to the left and right
        int halfSlots = (tapeW / 2) / slotW;

        for (int offset = -halfSlots - 1; offset <= halfSlots + 1; ++offset) {
            int moveIdx = currIdx + offset;
            int xPos = centerX + offset * slotW - slotW / 2; // center the slot

            // Ensure we are within tape bounds
            if (xPos + 5 >= tapeX && xPos + slotW - 5 <= tapeX + tapeW) {
                if (moveIdx >= 0 && moveIdx < (int)moves.size()) {
                    std::string mStr = moves[moveIdx].toString();
                    
                    if (offset == 0) {
                        // Current move: Highlight container and text
                        drawPanel(xPos, tapeY + 3, slotW, tapeH - 6, 1.0f, 0.55f, 0.0f, 0.25f);
                        drawBorder(xPos, tapeY + 3, slotW, tapeH - 6, 1.0f, 0.55f, 0.0f, 0.9f, 1.5f);
                        
                        // Center text in slot
                        int textX = xPos + (slotW - (int)mStr.length() * 8) / 2;
                        drawString(textX, tapeY + 22, mStr, GLUT_BITMAP_9_BY_15, 1.0f, 0.8f, 0.0f);
                    } else {
                        // Muted or normal moves
                        float r = 0.85f, g = 0.85f, b = 0.9f; // Future move (white)
                        if (offset < 0) {
                            r = 0.4f; g = 0.4f; b = 0.45f; // Past move (gray)
                        }
                        
                        int textX = xPos + (slotW - (int)mStr.length() * 8) / 2;
                        drawString(textX, tapeY + 22, mStr, GLUT_BITMAP_8_BY_13, r, g, b);
                    }
                } else if (moveIdx == (int)moves.size() && offset == 0) {
                    // Completed flag centered
                    std::string doneStr = "[SOLVED]";
                    int textX = centerX - 32;
                    drawPanel(centerX - 45, tapeY + 3, 90, tapeH - 6, 0.0f, 0.85f, 0.4f, 0.2f);
                    drawBorder(centerX - 45, tapeY + 3, 90, tapeH - 6, 0.0f, 0.85f, 0.4f, 0.8f, 1.5f);
                    drawString(textX, tapeY + 22, doneStr, GLUT_BITMAP_9_BY_15, 0.0f, 0.85f, 0.4f);
                } else if (moveIdx == -1 && currIdx == 0 && offset == -1) {
                    // Start marker
                    drawString(xPos + 10, tapeY + 22, "START", GLUT_BITMAP_HELVETICA_10, 0.4f, 0.4f, 0.45f);
                } else if (moveIdx == (int)moves.size() && offset == 1) {
                    // End marker
                    drawString(xPos + 15, tapeY + 22, "END", GLUT_BITMAP_HELVETICA_10, 0.6f, 0.6f, 0.7f);
                }
            }
        }

        // Draw small red vertical center cursor needle to highlight the current step
        glColor4f(1.0f, 0.3f, 0.3f, 0.8f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2i(centerX, tapeY + 1);
        glVertex2i(centerX, tapeY + tapeH - 2);
        glEnd();
        glLineWidth(1.0f);
    }

    // Restore viewing matrices and depth/light state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    
    restoreProjection();
}

void HUD::renderScorePanel(int width, int height, const ScoreManager& scoreManager, int cubeSize, bool practiceMode) {
    setupOrthographicProjection(width, height);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int panelW = 200;
    int panelX = width - panelW - 10;
    // Sits just below the Glass Cube badge (which occupies y = 10..38)
    int panelY = 48;

    char buf[64];

    if (scoreManager.isActive()) {
        // --- Live session: move counter, stopwatch, live score estimate ---
        int panelH = 92;
        drawPanel(panelX, panelY, panelW, panelH, 0.08f, 0.08f, 0.1f, 0.85f);
        drawBorder(panelX, panelY, panelW, panelH, 0.0f, 0.8f, 1.0f, 0.6f, 1.0f);

        drawString(panelX + 12, panelY + 18, "SOLVE IN PROGRESS", GLUT_BITMAP_HELVETICA_12, 1.0f, 0.75f, 0.2f);
        
        if (practiceMode) {
            drawPanel(panelX + 140, panelY + 6, 54, 14, 0.6f, 0.2f, 0.6f, 0.8f);
            drawString(panelX + 143, panelY + 16, "PRACTICE", GLUT_BITMAP_HELVETICA_10, 1.0f, 0.8f, 1.0f);
        }

        if (scoreManager.isPaused()) {
            drawString(panelX + 12, panelY + 40, "Time:  [PAUSED]", GLUT_BITMAP_8_BY_13, 1.0f, 0.55f, 0.0f);
        } else {
            double t = scoreManager.getElapsedSeconds();
            int mins = (int)(t / 60.0);
            double secs = t - mins * 60.0;
            snprintf(buf, sizeof(buf), "Time:  %02d:%05.2f", mins, secs);
            drawString(panelX + 12, panelY + 40, buf, GLUT_BITMAP_8_BY_13, 0.85f, 0.85f, 0.9f);
        }

        snprintf(buf, sizeof(buf), "Moves: %d", scoreManager.getMoveCount());
        drawString(panelX + 12, panelY + 58, buf, GLUT_BITMAP_8_BY_13, 0.85f, 0.85f, 0.9f);

        snprintf(buf, sizeof(buf), "Score: %d", scoreManager.getLiveScore());
        drawString(panelX + 12, panelY + 76, buf, GLUT_BITMAP_8_BY_13, 0.0f, 1.0f, 0.6f);
    } else if (scoreManager.hasLastResult()) {
        // --- Just finished: show result + personal best for this cube size ---
        const ScoreEntry& last = scoreManager.getLastResult();
        int best = scoreManager.getBestScore(cubeSize);
        bool isNewBest = (last.score >= best) && last.cubeSize == cubeSize;

        int panelH = 110;
        drawPanel(panelX, panelY, panelW, panelH, 0.08f, 0.08f, 0.1f, 0.85f);
        drawBorder(panelX, panelY, panelW, panelH,
                   isNewBest ? 0.0f : 0.0f, isNewBest ? 1.0f : 0.8f, isNewBest ? 0.4f : 1.0f, 0.7f, 1.5f);

        drawString(panelX + 12, panelY + 18, "SOLVE COMPLETE!", GLUT_BITMAP_HELVETICA_12, 1.0f, 0.75f, 0.2f);

        int mins = (int)(last.timeSeconds / 60.0);
        double secs = last.timeSeconds - mins * 60.0;
        snprintf(buf, sizeof(buf), "Time:  %02d:%05.2f", mins, secs);
        drawString(panelX + 12, panelY + 40, buf, GLUT_BITMAP_8_BY_13, 0.85f, 0.85f, 0.9f);

        snprintf(buf, sizeof(buf), "Moves: %d", last.moves);
        drawString(panelX + 12, panelY + 58, buf, GLUT_BITMAP_8_BY_13, 0.85f, 0.85f, 0.9f);

        snprintf(buf, sizeof(buf), "Score: %d", last.score);
        drawString(panelX + 12, panelY + 76, buf, GLUT_BITMAP_8_BY_13, 0.0f, 1.0f, 0.6f);

        if (isNewBest) {
            drawString(panelX + 12, panelY + 96, "NEW BEST for this size!", GLUT_BITMAP_HELVETICA_10, 1.0f, 0.85f, 0.0f);
        } else {
            snprintf(buf, sizeof(buf), "Best (this size): %d", best);
            drawString(panelX + 12, panelY + 96, buf, GLUT_BITMAP_HELVETICA_10, 0.6f, 0.6f, 0.7f);
        }
    } else {
        // --- Idle / no session yet ---
        int panelH = 44;
        drawPanel(panelX, panelY, panelW, panelH, 0.08f, 0.08f, 0.1f, 0.65f);
        drawBorder(panelX, panelY, panelW, panelH, 0.2f, 0.2f, 0.25f, 0.5f, 1.0f);
        drawString(panelX + 12, panelY + 18, "Press 'S' to scramble", GLUT_BITMAP_HELVETICA_10, 0.6f, 0.6f, 0.7f);

        int best = scoreManager.getBestScore(cubeSize);
        if (best > 0) {
            snprintf(buf, sizeof(buf), "Best (this size): %d", best);
            drawString(panelX + 12, panelY + 36, buf, GLUT_BITMAP_HELVETICA_10, 0.6f, 0.6f, 0.7f);
        }
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);

    restoreProjection();
}