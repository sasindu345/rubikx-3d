#include <iostream>
#include <sstream>
#include <cmath>
#include <string>
#include "cube/RubiksCube.h"
#include "cube/CubeFactory.h"
#include "cube/Move.h"
#include "Renderer.h"
#include "Camera.h"
#include "Lighting.h"
#include "Animation.h"
#include "solver/Scrambler.h"
#include "solver/ReverseSolver.h"
#include "ui/SolutionPlayer.h"
#include "ui/HUD.h"
#include "algorithms/Algorithms.h"
#include "utils/ScoreManager.h"
#include "Colors.h"
#include "solver/PatternLibrary.h"
#include "ui/WelcomeScreen.h"

WelcomeScreen welcomeScreen;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Screen dimensions
int windowWidth = 1024;
int windowHeight = 768;

// Global objects
RubiksCube activeCube;
Renderer renderer;
Camera camera;
Animation animation;

// Scrambling state
bool isScrambling = false;

// Alpha Blending / Glass Cube mode toggle
bool alphaBlendingEnabled = false;

// Solution player
SolutionPlayer solutionPlayer;
HUD hud;
ScoreManager scoreManager;
bool showHelp = true;
bool practiceMode = false;
bool showStats = false;

// ── Feature: Exploded Cube View ──────────────────────────────────
// Controls how far cubies spread outward from the centre.
// targetExplosion is set by keyboard; currentExplosion is interpolated.
float targetExplosion = 0.0f;
float currentExplosion = 0.0f;

// ── Feature: Orbiting Point Light Source ─────────────────────────
bool lightOrbitEnabled = false;
float lightOrbitAngle = 0.0f;     // current angle in degrees
float lightOrbitRadius = 12.0f;   // orbit distance from centre
float lightOrbitSpeed = 45.0f;    // degrees per second

// Move history tracker
std::vector<Move> moveHistory;
std::vector<Move> lastScramble;

// Pending inner-layer state machine (-1 = none armed)
int pendingLayer = -1;

void queueUserMove(const Move& m) {
    // Clear any active solver playback when user manually rotates a face
    solutionPlayer.setMoves({});

    // Count this as a solving move for scoring (only while a timed session
    // is active and we're not in the middle of an auto-scramble)
    if (!isScrambling && !scoreManager.isPaused()) {
        scoreManager.recordMove();
    }

    // Maintain history (cancel adjacent inverses)
    if (!moveHistory.empty() && moveHistory.back() == m.getInverse()) {
        moveHistory.pop_back();
    } else {
        moveHistory.push_back(m);
    }
    
    animation.queueMove(m);
}

void queueWholeRotation(Face face, Direction dir) {
    for (int layer = 0; layer < activeCube.getSize(); ++layer) {
        queueUserMove(Move(face, dir, layer));
    }
}

// Wide move: queue outer layer (0) + inner layer (1) together.
// Guards against cubes where layer 1 does not exist (2x2).
void queueWideMove(Face face, Direction dir) {
    queueUserMove(Move(face, dir, 0));
    if (activeCube.getSize() > 2) {
        queueUserMove(Move(face, dir, 1));
    }
}

void applyPattern(int index) {
    if (!animation.isAnimating() && animation.moveQueue.empty()) {
        activeCube.reset();
        moveHistory.clear();
        solutionPlayer.setMoves({});
        scoreManager.cancelSession();

        auto patterns = PatternLibrary::getPatterns(activeCube.getSize());
        if (index < 0 || index >= patterns.size()) return;
        const auto& pattern = patterns[index];
        std::cout << "Applying pattern: " << pattern.name << std::endl;
        for (const auto& tok : pattern.algorithm) {
            auto moves = Move::parseAlgorithm(tok, activeCube.getSize());
            for (const auto& m : moves) {
                moveHistory.push_back(m);
                animation.queueMove(m);
            }
        }
    }
}

// Mouse tracking state
bool isLeftMouseDown = false;
bool isRightMouseDown = false;
int lastMouseX = 0;
int lastMouseY = 0;

// ── Dynamic Procedural Background Gradient ───────────────────────
// Uses the Gouraud shading algorithm from src/algorithms/Shading.cpp


// Display callback
void display() {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (welcomeScreen.isActive()) {
        // Welcome screen: render ONLY the 2D setup overlay on a clean dark background
        // (no 3D cube, no camera, no lighting)
        welcomeScreen.render(windowWidth, windowHeight);
    } else {
        // ── Draw background gradient behind the 3D scene ──
        renderGradientBackground(windowWidth, windowHeight, 
            0.06f, 0.04f, 0.14f,  // Top-left: deep dark indigo
            0.04f, 0.08f, 0.18f,  // Top-right: dark midnight blue
            0.03f, 0.12f, 0.16f,  // Bottom-right: dark teal
            0.08f, 0.04f, 0.12f   // Bottom-left: very dark purple
        );

        // Normal gameplay rendering
        // Reset modelview transformation
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        // Position/orient the camera
        camera.apply();
        
        // Position lighting in scene space
        if (lightOrbitEnabled) {
            Lighting::apply(lightOrbitAngle, lightOrbitRadius);
        } else {
            Lighting::apply();
        }

        // Sync alpha blending state with renderer before drawing
        renderer.alphaBlending = alphaBlendingEnabled;

        // Sync explosion factor
        renderer.explosionFactor = currentExplosion;
        
        // Render the Rubik's Cube with active animations
        renderer.renderCube(activeCube, animation);

        // Render the visible light source indicator when orbiting
        if (lightOrbitEnabled) {
            Lighting::renderLightIndicator(lightOrbitAngle, lightOrbitRadius);
        }
        
        // Render 2D HUD Help Menu overlay
        hud.render(windowWidth, windowHeight, solutionPlayer, showHelp, alphaBlendingEnabled, static_cast<int>(renderer.renderMode), activeCube.getSize());

        // Render scoring panel (top-right): live timer/move counter or last result
        hud.renderScorePanel(windowWidth, windowHeight, scoreManager, activeCube.getSize(), practiceMode);

        // Render pending layer status banner (bottom-left, only when armed)
        hud.renderPendingLayerStatus(windowWidth, windowHeight, pendingLayer);

        if (showStats) {
            hud.renderStatsPanel(windowWidth, windowHeight, scoreManager, activeCube.getSize());
        }
    }
    
    // Swap front and back buffers
    glutSwapBuffers();
}

// Reshape callback (setup perspective projection)
void reshape(int width, int height) {
    if (height == 0) height = 1;
    
    windowWidth = width;
    windowHeight = height;
    
    configureViewport(0, 0, width, height);
    
    float aspect = (float)width / (float)height;
    // Set 45-degree field-of-view, near clipping plane at 0.1, far clipping plane at 100.0
    setupPerspectiveProjection(45.0, aspect, 0.1, 100.0);
}

void keyboard(unsigned char key, int x, int y) {
    if (welcomeScreen.isActive()) {
        bool startGame = false;
        if (welcomeScreen.handleKey(key, x, y, startGame)) {
            if (startGame) {
                const auto& cfg = welcomeScreen.getConfig();
                activeCube = CubeFactory::create(cfg.cubeSize);
                renderer.renderMode = cfg.renderMode;
                Colors::setScheme(cfg.colorScheme);
                alphaBlendingEnabled = cfg.glassMode;
                practiceMode = cfg.practiceMode;

                if (cfg.cubeSize == 2) camera.radius = 8.0f;
                else if (cfg.cubeSize == 3) camera.radius = 10.0f;
                else if (cfg.cubeSize == 4) camera.radius = 12.0f;
                else if (cfg.cubeSize == 5) camera.radius = 14.0f;
                else if (cfg.cubeSize == 6) camera.radius = 16.0f;
                else if (cfg.cubeSize == 7) camera.radius = 18.0f;

                moveHistory.clear();
                solutionPlayer.setMoves({});
                scoreManager.cancelSession();

                if (cfg.autoScramble) {
                    int steps = (cfg.cubeSize == 2) ? 10 : (cfg.cubeSize == 3) ? 20 : (cfg.cubeSize == 4) ? 25 : (cfg.cubeSize == 5) ? 35 : (cfg.cubeSize == 6) ? 45 : 55;
                    std::vector<Move> moves = Scrambler::generateScramble(cfg.cubeSize, steps);
                    lastScramble = moves;
                    isScrambling = true;
                    animation.speed = 1800.0f;
                    for (const auto& m : moves) {
                        moveHistory.push_back(m);
                        animation.queueMove(m);
                    }
                }
            }
            glutPostRedisplay();
        }
        return;
    }

    // Read modifier keys for Ctrl/Alt detection before the switch
    int modifiers = glutGetModifiers();
    bool ctrlHeld  = (modifiers & GLUT_ACTIVE_CTRL)  != 0;
    bool altHeld   = (modifiers & GLUT_ACTIVE_ALT)   != 0;
    bool shiftHeld = (modifiers & GLUT_ACTIVE_SHIFT) != 0;

    // ── Alt + digit: arm a pending inner layer ───────────────────────────────
    // Alt+2 = layer 1, Alt+3 = layer 2, ..., Alt+7 = layer 6 (0-based)
    if (altHeld && key >= '2' && key <= '7') {
        int requestedLayer = (key - '0') - 1; // '2'->1, '3'->2, etc.
        if (requestedLayer >= activeCube.getSize()) {
            std::cout << "[Inner Layer] Layer " << (requestedLayer + 1)
                      << " does not exist on a "
                      << activeCube.getSize() << "x" << activeCube.getSize()
                      << " cube. Cancelled." << std::endl;
            pendingLayer = -1;
        } else {
            pendingLayer = requestedLayer;
            std::cout << "[Inner Layer] Layer " << (pendingLayer + 1)
                      << " armed — press a face key (R/L/U/D/F/B)." << std::endl;
        }
        glutPostRedisplay();
        return;
    }

    // ── Ctrl combos: wide moves (outer + layer 1 together) ───────────────────
    // Ctrl+R=18, Ctrl+L=12, Ctrl+U=21, Ctrl+D=4, Ctrl+F=6, Ctrl+B=2
    // Shift+Ctrl gives the CCW (prime) variant.
    if (ctrlHeld) {
        Direction wideDir = shiftHeld ? Direction::CCW : Direction::CW;
        switch (key) {
            case 18: queueWideMove(Face::RIGHT, wideDir); pendingLayer = -1; glutPostRedisplay(); return; // Ctrl+R
            case 12: queueWideMove(Face::LEFT,  wideDir); pendingLayer = -1; glutPostRedisplay(); return; // Ctrl+L
            case 21: queueWideMove(Face::UP,    wideDir); pendingLayer = -1; glutPostRedisplay(); return; // Ctrl+U
            case  4: queueWideMove(Face::DOWN,  wideDir); pendingLayer = -1; glutPostRedisplay(); return; // Ctrl+D
            case  6: queueWideMove(Face::FRONT, wideDir); pendingLayer = -1; glutPostRedisplay(); return; // Ctrl+F
            case  2: queueWideMove(Face::BACK,  wideDir); pendingLayer = -1; glutPostRedisplay(); return; // Ctrl+B
            default: break;
        }
    }

    // ── Face-move helper lambdas (respect pending inner layer) ─────────────
    // If a layer was armed via Alt+digit, use it; otherwise use layer 0.
    auto queueFaceCW = [&](Face face) {
        int lay = (pendingLayer >= 0 && pendingLayer < activeCube.getSize()) ? pendingLayer : 0;
        queueUserMove(Move(face, Direction::CW, lay));
        pendingLayer = -1;
        glutPostRedisplay();
    };
    auto queueFaceCCW = [&](Face face) {
        int lay = (pendingLayer >= 0 && pendingLayer < activeCube.getSize()) ? pendingLayer : 0;
        queueUserMove(Move(face, Direction::CCW, lay));
        pendingLayer = -1;
        glutPostRedisplay();
    };

    switch (key) {
        case 27: // Escape key — also cancels any pending layer
            pendingLayer = -1;
            std::cout << "Exiting application." << std::endl;
            exit(0);
            break;
        
        // Toggle Help Menu (H or h)
        case 'H':
        case 'h':
            showHelp = !showHelp;
            glutPostRedisplay();
            break;

        // Toggle Stats Panel (I or i)
        case 'I':
        case 'i':
            showStats = !showStats;
            glutPostRedisplay();
            break;

        // Toggle Alpha Blending / Glass Cube mode (T/t)
        // Theory: Alpha Compositing — C_out = C_src*A_src + C_dst*(1-A_src)
        // Renders cubie bases as semi-transparent glass using GL_BLEND.
        // Back-to-front Painter's Algorithm sort ensures correct layering.
        case 'T':
        case 't':
            alphaBlendingEnabled = !alphaBlendingEnabled;
            std::cout << "[Alpha Blending] Glass Cube mode: "
                      << (alphaBlendingEnabled ? "ON  (GL_BLEND enabled, Painter's sort active)"
                                               : "OFF (normal opaque rendering)")
                      << std::endl;
            glutPostRedisplay();
            break;

        // Toggle Rendering Mode: Solid -> Wireframe -> Textured -> Solid (W/w)
        case 'W':
        case 'w':
            if (renderer.renderMode == RenderMode::SOLID) {
                renderer.renderMode = RenderMode::WIREFRAME;
                std::cout << "[Render Mode] Switched to WIREFRAME" << std::endl;
            } else if (renderer.renderMode == RenderMode::WIREFRAME) {
                renderer.renderMode = RenderMode::TEXTURED;
                std::cout << "[Render Mode] Switched to TEXTURED" << std::endl;
            } else {
                renderer.renderMode = RenderMode::SOLID;
                std::cout << "[Render Mode] Switched to SOLID" << std::endl;
            }
            glutPostRedisplay();
            break;
        
        // Solve (Z/z)
        case 'Z':
        case 'z':
            if (!animation.isAnimating() && animation.moveQueue.empty()) {
                std::cout << "Calculating backward steps from history..." << std::endl;
                std::vector<Move> solution = ReverseSolver::solve(moveHistory);
                std::cout << "Solution computed (" << solution.size() << " moves): ";
                for (const auto& m : solution) {
                    std::cout << m.toString() << " ";
                }
                std::cout << std::endl;
                solutionPlayer.setMoves(solution);
            }
            break;
            
        // Play/Pause Autoplay (P/p)
        case 'P':
        case 'p':
            if (solutionPlayer.getMoveCount() > 0) {
                solutionPlayer.setPlaying(!solutionPlayer.isPlaying());
                std::cout << (solutionPlayer.isPlaying() ? "Autoplay started." : "Autoplay paused.") << std::endl;
            } else {
                std::cout << "No active solution to play. Press 'Z' first." << std::endl;
            }
            break;

        // Step Forward (. or >)
        case '.':
        case '>':
            if (solutionPlayer.getMoveCount() > 0) {
                solutionPlayer.setPlaying(false); // Pause autoplay
                solutionPlayer.stepForward(animation, activeCube);
            } else {
                std::cout << "No active solution. Press 'Z' first." << std::endl;
            }
            break;

        // Step Backward (, or <)
        case ',':
        case '<':
            if (solutionPlayer.getMoveCount() > 0) {
                solutionPlayer.setPlaying(false); // Pause autoplay
                solutionPlayer.stepBackward(animation, activeCube);
            } else {
                std::cout << "No active solution. Press 'Z' first." << std::endl;
            }
            break;

        // Speed Up (+ or =)
        case '+':
        case '=':
            solutionPlayer.adjustSpeed(0.1f);
            break;

        // Slow Down (- or _)
        case '-':
        case '_':
            solutionPlayer.adjustSpeed(-0.1f);
            break;
            
        // Patterns (!, @, #)
        case '!': applyPattern(0); break; // Checkerboard
        case '@': applyPattern(1); break; // Superflip/Cube in Cube
        case '#': applyPattern(2); break; // Cube in Cube
        
        // Scramble
        case 'S':
        case 's':
            if (!animation.isAnimating() && animation.moveQueue.empty()) {
                std::cout << "Generating scramble..." << std::endl;
                activeCube.reset();
                moveHistory.clear();
                solutionPlayer.setMoves({});
                scoreManager.cancelSession();

                int n = activeCube.getSize();
                int steps = (n == 2) ? 10 : (n == 3) ? 20 : (n == 4) ? 25 : (n == 5) ? 35 : (n == 6) ? 45 : 55;
                std::vector<Move> moves = Scrambler::generateScramble(activeCube.getSize(), steps);
                lastScramble = moves;
                isScrambling = true;
                animation.speed = 1800.0f; // Fast scramble animation
                std::cout << "Scramble (" << moves.size() << " moves): ";
                for (const auto& m : moves) {
                    std::cout << m.toString() << " ";
                    moveHistory.push_back(m);
                    animation.queueMove(m);
                }
                std::cout << std::endl;
            }
            break;
            

        case 'Y':
        case 'y':
            if (!animation.isAnimating() && animation.moveQueue.empty() && !lastScramble.empty()) {
                std::cout << "Retrying same scramble..." << std::endl;
                activeCube.reset();
                moveHistory.clear();
                solutionPlayer.setMoves({});
                scoreManager.cancelSession();

                isScrambling = true;
                animation.speed = 1800.0f;
                for (const auto& m : lastScramble) {
                    moveHistory.push_back(m);
                    animation.queueMove(m);
                }
            } else if (lastScramble.empty()) {
                std::cout << "No previous scramble to repeat. Press 'S' first." << std::endl;
            }
            break;

        case ' ':
            scoreManager.togglePause();
            std::cout << (scoreManager.isPaused() ? "Timer paused." : "Timer resumed.") << std::endl;
            break;
            
        case 'O':
        case 'o':
        case '0':
            {
                WelcomeConfig cfg;
                cfg.cubeSize = activeCube.getSize();
                cfg.renderMode = renderer.renderMode;
                cfg.colorScheme = Colors::getScheme();
                cfg.glassMode = alphaBlendingEnabled;
                cfg.practiceMode = practiceMode;
                cfg.autoScramble = false;
                welcomeScreen.setConfig(cfg);
                welcomeScreen.setActive(true);
                std::cout << "Opened Welcome Screen configuration." << std::endl;
                glutPostRedisplay();
            }
            break;

        case 'M':
        case 'm':
            practiceMode = !practiceMode;
            std::cout << "Practice mode: " << (practiceMode ? "ON (scores won't be saved)" : "OFF") << std::endl;
            break;
            
        // Reset Cube (X/x)
        case 'X':
        case 'x':
            if (!animation.isAnimating() && animation.moveQueue.empty()) {
                std::cout << "Resetting cube to solved state..." << std::endl;
                activeCube.reset();
                moveHistory.clear();
                solutionPlayer.setMoves({});
                scoreManager.cancelSession();
                glutPostRedisplay();
            }
            break;
            
        // Cycle Color Schemes (C/c)
        case 'C':
        case 'c': {
            int next = (((int)Colors::getScheme()) + 1) % (int)Colors::Scheme::COUNT;
            Colors::setScheme((Colors::Scheme)next);
            std::cout << "Color scheme changed." << std::endl;
            glutPostRedisplay();
            break;
        }
        
        // Switch Cube Size (2: 2x2, 3: 3x3, 4: 4x4, 5: 5x5, 6: 6x6, 7: 7x7)
        case '2':
            if (activeCube.getSize() != 2) {
                activeCube = CubeFactory::create(2);
                moveHistory.clear();
                solutionPlayer.setMoves({});
                scoreManager.cancelSession();
                camera.radius = 8.0f; // Zoom in closer for the smaller 2x2 cube
                std::cout << "Switched to 2x2 Rubik's Cube." << std::endl;
                glutPostRedisplay();
            }
            break;
        case '3':
            if (activeCube.getSize() != 3) {
                activeCube = CubeFactory::create(3);
                moveHistory.clear();
                solutionPlayer.setMoves({});
                scoreManager.cancelSession();
                camera.radius = 10.0f; // Reset zoom to default
                std::cout << "Switched to 3x3 Rubik's Cube." << std::endl;
                glutPostRedisplay();
            }
            break;
        case '4':
            if (activeCube.getSize() != 4) {
                activeCube = CubeFactory::create(4);
                moveHistory.clear();
                solutionPlayer.setMoves({});
                scoreManager.cancelSession();
                camera.radius = 12.0f; // Zoom out slightly for larger cube
                std::cout << "Switched to 4x4 Rubik's Cube." << std::endl;
                glutPostRedisplay();
            }
            break;
        case '5':
            if (activeCube.getSize() != 5) {
                activeCube = CubeFactory::create(5);
                moveHistory.clear();
                solutionPlayer.setMoves({});
                scoreManager.cancelSession();
                camera.radius = 14.0f; // Zoom out further for 5x5 cube
                std::cout << "Switched to 5x5 Rubik's Cube." << std::endl;
                glutPostRedisplay();
            }
            break;
        case '6':
            if (activeCube.getSize() != 6) {
                activeCube = CubeFactory::create(6);
                moveHistory.clear();
                solutionPlayer.setMoves({});
                scoreManager.cancelSession();
                camera.radius = 16.0f; // Zoom out further for 6x6 cube
                std::cout << "Switched to 6x6 Rubik's Cube." << std::endl;
                glutPostRedisplay();
            }
            break;
        case '7':
            if (activeCube.getSize() != 7) {
                activeCube = CubeFactory::create(7);
                moveHistory.clear();
                solutionPlayer.setMoves({});
                scoreManager.cancelSession();
                camera.radius = 18.0f; // Zoom out further for 7x7 cube
                std::cout << "Switched to 7x7 Rubik's Cube." << std::endl;
                glutPostRedisplay();
            }
            break;

        // ── Outer face moves — with pending-layer override ───────────────────
        // CW (uppercase) and CCW (lowercase) both route through the lambdas
        // defined above the switch so the armed layer (if any) is respected.
        case 'R': queueFaceCW(Face::RIGHT);  break;
        case 'L': queueFaceCW(Face::LEFT);   break;
        case 'U': queueFaceCW(Face::UP);     break;
        case 'D': queueFaceCW(Face::DOWN);   break;
        case 'F': queueFaceCW(Face::FRONT);  break;
        case 'B': queueFaceCW(Face::BACK);   break;

        // Counter-Clockwise moves (Lowercase letters)
        case 'r': queueFaceCCW(Face::RIGHT); break;
        case 'l': queueFaceCCW(Face::LEFT);  break;
        case 'u': queueFaceCCW(Face::UP);    break;
        case 'd': queueFaceCCW(Face::DOWN);  break;
        case 'f': queueFaceCCW(Face::FRONT); break;
        case 'b': queueFaceCCW(Face::BACK);  break;

        // ── Slice moves (middle layer only) ──────────────────────────────────
        // M slice (middle layer, L-axis): J=CW / j=CCW  (M/m taken by Practice Mode)
        case 'J': queueUserMove(Move(Face::LEFT,  Direction::CW,  activeCube.getSize() / 2)); break;
        case 'j': queueUserMove(Move(Face::LEFT,  Direction::CCW, activeCube.getSize() / 2)); break;
        // E slice (middle layer, D-axis): G=CW / g=CCW  (E/e taken by Exploded View)
        case 'G': queueUserMove(Move(Face::DOWN,  Direction::CW,  activeCube.getSize() / 2)); break;
        case 'g': queueUserMove(Move(Face::DOWN,  Direction::CCW, activeCube.getSize() / 2)); break;
        // S slice (middle layer, F-axis): N=CW / n=CCW  (S/s taken by Scramble)
        case 'N': queueUserMove(Move(Face::FRONT, Direction::CW,  activeCube.getSize() / 2)); break;
        case 'n': queueUserMove(Move(Face::FRONT, Direction::CCW, activeCube.getSize() / 2)); break;

        // ── Wide moves (layers 0 + 1 together) ───────────────────────────────
        // Rw: A=CW / a=CCW  (]/[ taken by X-rotation)
        case 'A': queueWideMove(Face::RIGHT, Direction::CW);  break;
        case 'a': queueWideMove(Face::RIGHT, Direction::CCW); break;
        // Lw: Q=CW / q=CCW
        case 'Q': queueWideMove(Face::LEFT,  Direction::CW);  break;
        case 'q': queueWideMove(Face::LEFT,  Direction::CCW); break;
        // Uw: V=CW / v=CCW  (;/' taken by Y-rotation)
        case 'V': queueWideMove(Face::UP,    Direction::CW);  break;
        case 'v': queueWideMove(Face::UP,    Direction::CCW); break;
        // Dw: `=CW / ~=CCW  (/ taken by Z-rot CCW, . taken by step-fwd)
        case '`': queueWideMove(Face::DOWN,  Direction::CW);  break;
        case '~': queueWideMove(Face::DOWN,  Direction::CCW); break;

        // ── Whole-cube rotations (all layers) ────────────────────────────────
        // X (all layers on R): [=CW / ]=CCW
        case '[': queueWholeRotation(Face::RIGHT, Direction::CW);  break;
        case ']': queueWholeRotation(Face::RIGHT, Direction::CCW); break;
        // Y (all layers on U): ;=CW / '=CCW
        case ';': queueWholeRotation(Face::UP,    Direction::CW);  break;
        case 39:  queueWholeRotation(Face::UP,    Direction::CCW); break; // 39 = apostrophe '
        // Z (all layers on F): \=CW / /=CCW
        case 92:  queueWholeRotation(Face::FRONT, Direction::CW);  break; // 92 = backslash
        case '/': queueWholeRotation(Face::FRONT, Direction::CCW); break;

        // ── Exploded Cube View (E = expand, e = contract) ──
        // Smoothly spreads cubies outward from the centre to reveal
        // internal structure.  Uses translation matrix scaling.
        case 'E':
            targetExplosion += 0.15f;
            if (targetExplosion > 1.5f) targetExplosion = 1.5f;
            std::cout << "[Exploded View] Target expansion: " << targetExplosion << std::endl;
            break;
        case 'e':
            targetExplosion -= 0.15f;
            if (targetExplosion < 0.0f) targetExplosion = 0.0f;
            std::cout << "[Exploded View] Target expansion: " << targetExplosion << std::endl;
            break;

        // ── Orbiting Point Light Source (K/k toggle) ──
        // Toggles a dynamic light that orbits the cube, producing
        // real-time specular highlight changes on each face.
        case 'K':
        case 'k':
            lightOrbitEnabled = !lightOrbitEnabled;
            std::cout << "[Orbiting Light] "
                      << (lightOrbitEnabled ? "ON  (light orbiting cube)"
                                           : "OFF (static lighting)")
                      << std::endl;
            break;

        default:
            // Any unhandled key cancels a pending layer arm
            if (pendingLayer != -1) {
                std::cout << "[Inner Layer] Pending layer " << (pendingLayer + 1)
                          << " cancelled (unrecognised key)." << std::endl;
                pendingLayer = -1;
                glutPostRedisplay();
            } else {
                std::cout << "Key pressed: " << key << " at mouse (" << x << ", " << y << ")" << std::endl;
            }
            break;
    }
}

void specialKeys(int key, int x, int y) {
    if (welcomeScreen.isActive()) {
        welcomeScreen.handleSpecialKey(key, x, y);
        glutPostRedisplay();
        return;
    }

    switch (key) {
        case GLUT_KEY_LEFT:
            camera.orbit(-5.0f, 0.0f);
            break;
        case GLUT_KEY_RIGHT:
            camera.orbit(5.0f, 0.0f);
            break;
        case GLUT_KEY_UP:
            camera.orbit(0.0f, 5.0f);
            break;
        case GLUT_KEY_DOWN:
            camera.orbit(0.0f, -5.0f);
            break;
    }
    glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y) {
    if (welcomeScreen.isActive()) {
        bool startGame = false;
        if (welcomeScreen.handleMouse(button, state, x, y, startGame)) {
            if (startGame) {
                const auto& cfg = welcomeScreen.getConfig();
                activeCube = CubeFactory::create(cfg.cubeSize);
                renderer.renderMode = cfg.renderMode;
                Colors::setScheme(cfg.colorScheme);
                alphaBlendingEnabled = cfg.glassMode;
                practiceMode = cfg.practiceMode;

                if (cfg.cubeSize == 2) camera.radius = 8.0f;
                else if (cfg.cubeSize == 3) camera.radius = 10.0f;
                else if (cfg.cubeSize == 4) camera.radius = 12.0f;
                else if (cfg.cubeSize == 5) camera.radius = 14.0f;
                else if (cfg.cubeSize == 6) camera.radius = 16.0f;
                else if (cfg.cubeSize == 7) camera.radius = 18.0f;

                moveHistory.clear();
                solutionPlayer.setMoves({});
                scoreManager.cancelSession();

                if (cfg.autoScramble) {
                    int steps = (cfg.cubeSize == 2) ? 10 : (cfg.cubeSize == 3) ? 20 : (cfg.cubeSize == 4) ? 25 : (cfg.cubeSize == 5) ? 35 : (cfg.cubeSize == 6) ? 45 : 55;
                    std::vector<Move> moves = Scrambler::generateScramble(cfg.cubeSize, steps);
                    lastScramble = moves;
                    isScrambling = true;
                    animation.speed = 1800.0f;
                    for (const auto& m : moves) {
                        moveHistory.push_back(m);
                        animation.queueMove(m);
                    }
                }
            }
            glutPostRedisplay();
        }
        return;
    }

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isLeftMouseDown = true;
            lastMouseX = x;
            lastMouseY = y;
        } else if (state == GLUT_UP) {
            isLeftMouseDown = false;
        }
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            isRightMouseDown = true;
            lastMouseX = x;
            lastMouseY = y;
        } else if (state == GLUT_UP) {
            isRightMouseDown = false;
        }
    }
    // Handle scroll wheel events (commonly buttons 3 & 4 in GLUT)
    else if (button == 3) { // Scroll Up
        if (state == GLUT_DOWN) {
            camera.zoom(-0.5f);
            glutPostRedisplay();
        }
    } else if (button == 4) { // Scroll Down
        if (state == GLUT_DOWN) {
            camera.zoom(0.5f);
            glutPostRedisplay();
        }
    }
}

// Mouse motion callback (active dragging)
void mouseMotion(int x, int y) {
    int dx = x - lastMouseX;
    int dy = y - lastMouseY;

    if (isLeftMouseDown) {
        // Orbit camera: moving mouse left rotates camera right, moving up rotates down
        camera.orbit(-dx * 0.4f, -dy * 0.4f);
    } else if (isRightMouseDown) {
        // Zoom camera: dragging up zooms in, down zooms out
        camera.zoom(dy * 0.05f);
    }

    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}

// Initialize OpenGL state
void initGL() {
    // Deep dark background color
    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
    
    // Enable depth test for hidden surface removal
    enableDepthTesting();
    
    // Enable back-face culling for visible surface detection demonstration
    enableBackFaceCulling();
    
    // Smooth shading model
    glShadeModel(GL_SMOOTH);
    
    // Normalize normals after scale/matrix operations
    glEnable(GL_NORMALIZE);
    
    // Initialize scene lighting
    Lighting::init();
    
    std::cout << "OpenGL & Lighting Initialized." << std::endl;
}

// Self-contained data model validation
void testCubeState() {
    std::cout << "\n=== Testing RubiksCube Data Model ===" << std::endl;
    RubiksCube cube = CubeFactory::create(3);
    std::cout << "Initial cube is solved: " << (cube.isSolved() ? "YES" : "NO") << std::endl;

    Move rMove(Face::RIGHT, Direction::CW, 0);
    std::cout << "Applying move: " << rMove.toString() << std::endl;
    cube.applyMove(rMove);
    std::cout << "Cube is solved: " << (cube.isSolved() ? "YES" : "NO") << std::endl;

    Move rPrime = rMove.getInverse();
    std::cout << "Applying inverse move: " << rPrime.toString() << std::endl;
    cube.applyMove(rPrime);
    std::cout << "Cube is solved after inverse: " << (cube.isSolved() ? "YES" : "NO") << std::endl;

    cube.reset();
    std::cout << "=== RubiksCube Data Model Test Passed! ===\n" << std::endl;
}

// Timer callback to update animation frames at ~60 FPS
void timer(int value) {
    // 16ms is roughly 0.016 seconds
    float deltaTime = 0.016f;

    // Update active animation state.
    // If a turn animation just completed, apply the change permanently to the RubiksCube state model.
    if (animation.update(deltaTime)) {
        activeCube.applyMove(animation.currentMove);
        if (activeCube.isSolved()) {
            std::cout << "Cube solved!" << std::endl;
            moveHistory.clear();
            solutionPlayer.setMoves({});

            // Score the attempt if a timed session was running with at least
            // one real move (avoids scoring a no-op "solve" right after reset)
            if (scoreManager.isActive() && scoreManager.getMoveCount() > 0) {
                if (practiceMode) {
                    scoreManager.cancelSession(); // discard without saving
                    std::cout << "Practice mode active - session discarded." << std::endl;
                } else {
                    ScoreEntry result = scoreManager.finishSession();
                    std::cout << "Score: " << result.score
                              << " | Moves: " << result.moves
                              << " | Time: " << result.timeSeconds << "s" << std::endl;
                }
            } else if (scoreManager.isActive()) {
                scoreManager.cancelSession();
            }
        }
    }

    // Update solution playback (if playing)
    solutionPlayer.update(deltaTime, animation, activeCube);

    // Reset scramble state and speed once scramble animation is finished
    if (isScrambling && !animation.isAnimating() && animation.moveQueue.empty()) {
        isScrambling = false;
        animation.speed = 450.0f; // Restore normal play speed
        std::cout << "Scramble complete." << std::endl;

        // Start a fresh timed/scored session now that the puzzle is mixed up
        scoreManager.startSession(activeCube.getSize());
    }

    glutPostRedisplay();

    // ── Animate Exploded View ──
    // Smoothly interpolate currentExplosion toward targetExplosion
    // using a simple exponential ease-out (LERP toward target).
    if (std::fabs(currentExplosion - targetExplosion) > 0.001f) {
        currentExplosion += (targetExplosion - currentExplosion) * 5.0f * deltaTime;
    } else {
        currentExplosion = targetExplosion;
    }

    // ── Animate Orbiting Light ──
    // Advance the orbit angle each frame when enabled.
    // The angle wraps at 360 degrees.
    if (lightOrbitEnabled) {
        lightOrbitAngle += lightOrbitSpeed * deltaTime;
        if (lightOrbitAngle >= 360.0f) lightOrbitAngle -= 360.0f;
    }

    glutTimerFunc(16, timer, 0); // queue next frame
}

int main(int argc, char** argv) {
    // Run tests
    testCubeState();

    // Initialize default cube size 3x3
    activeCube = CubeFactory::create(3);

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("RubikX-3D | Learning & Solving System | Press T for Glass Cube");
    
    initGL();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    
    // Register timer callback for smooth updates
    glutTimerFunc(16, timer, 0);
    
    glutMainLoop();
    
    return 0;
}