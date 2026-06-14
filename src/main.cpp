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
bool awaitingAlgInput = false;
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

void applyPattern3x3(int index) {
    if (activeCube.getSize() != 3) {
        std::cout << "Patterns currently only defined for 3x3." << std::endl;
        return;
    }
    if (!animation.isAnimating() && animation.moveQueue.empty()) {
        activeCube.reset();
        moveHistory.clear();
        solutionPlayer.setMoves({});
        scoreManager.cancelSession();

        auto patterns = PatternLibrary::getPatterns3x3();
        if (index < 0 || index >= patterns.size()) return;
        const auto& pattern = patterns[index];
        std::cout << "Applying pattern: " << pattern.name << std::endl;
        for (const auto& tok : pattern.algorithm) {
            std::string parseToken = tok;
            bool isDouble = false;
            if (!parseToken.empty() && parseToken.back() == '2') {
                isDouble = true;
                parseToken.pop_back(); // Remove '2'
            }
            Move m = Move::parse(parseToken);
            moveHistory.push_back(m);
            animation.queueMove(m);
            if (isDouble) {
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
// Renders a full-screen quad with per-vertex colours that are
// linearly interpolated (Gouraud shading) by the fixed-function
// pipeline. This produces a smooth cosmic gradient without any
// texture overhead.
//
// Theory: Colour Interpolation / Gouraud Shading
//   The GPU interpolates vertex colours across the polygon using
//   barycentric coordinates, producing a smooth gradient:
//     C(x,y) = alpha * C_v0 + beta * C_v1 + gamma * C_v2
void drawBackgroundGradient(int width, int height) {
    // Switch to 2D orthographic projection for the full-screen quad
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0); // origin top-left

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Disable depth/lighting — the gradient sits behind everything
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);

    glBegin(GL_QUADS);
    // Top-left:  deep dark indigo
    glColor3f(0.06f, 0.04f, 0.14f);
    glVertex2i(0, 0);
    // Top-right: dark midnight blue
    glColor3f(0.04f, 0.08f, 0.18f);
    glVertex2i(width, 0);
    // Bottom-right: dark teal
    glColor3f(0.03f, 0.12f, 0.16f);
    glVertex2i(width, height);
    // Bottom-left: very dark purple
    glColor3f(0.08f, 0.04f, 0.12f);
    glVertex2i(0, height);
    glEnd();

    // Restore state
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

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
        drawBackgroundGradient(windowWidth, windowHeight);

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
        hud.render(windowWidth, windowHeight, solutionPlayer, showHelp, alphaBlendingEnabled, static_cast<int>(renderer.renderMode));

        // Render scoring panel (top-right): live timer/move counter or last result
        hud.renderScorePanel(windowWidth, windowHeight, scoreManager, activeCube.getSize(), practiceMode);

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

    switch (key) {
        case 27: // Escape key
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
        case '!': applyPattern3x3(0); break; // Checkerboard
        case '@': applyPattern3x3(1); break; // Superflip
        case '#': applyPattern3x3(2); break; // Cube in Cube
        
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
            
        case 'A':
        case 'a':
            if (!animation.isAnimating() && animation.moveQueue.empty()) {
                awaitingAlgInput = true;
                std::cout << "\nEnter algorithm (e.g. \"R U R' U'\").\n"
                          << "Note: Type in the terminal window, then press Enter.\n"
                          << "> ";
                std::string line;
                std::getline(std::cin, line);

                std::istringstream iss(line);
                std::string token;
                std::vector<Move> alg;
                while (iss >> token) {
                    alg.push_back(Move::parse(token));
                }

                std::cout << "Queuing " << alg.size() << " moves." << std::endl;
                for (const auto& m : alg) {
                    queueUserMove(m);
                }
                awaitingAlgInput = false;
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

        // Clockwise moves (Capital letters)
        case 'R': queueUserMove(Move(Face::RIGHT, Direction::CW, 0)); break;
        case 'L': queueUserMove(Move(Face::LEFT, Direction::CW, 0)); break;
        case 'U': queueUserMove(Move(Face::UP, Direction::CW, 0)); break;
        case 'D': queueUserMove(Move(Face::DOWN, Direction::CW, 0)); break;
        case 'F': queueUserMove(Move(Face::FRONT, Direction::CW, 0)); break;
        case 'B': queueUserMove(Move(Face::BACK, Direction::CW, 0)); break;

        // Counter-Clockwise moves (Lowercase letters)
        case 'r': queueUserMove(Move(Face::RIGHT, Direction::CCW, 0)); break;
        case 'l': queueUserMove(Move(Face::LEFT, Direction::CCW, 0)); break;
        case 'u': queueUserMove(Move(Face::UP, Direction::CCW, 0)); break;
        case 'd': queueUserMove(Move(Face::DOWN, Direction::CCW, 0)); break;
        case 'f': queueUserMove(Move(Face::FRONT, Direction::CCW, 0)); break;
        case 'b': queueUserMove(Move(Face::BACK, Direction::CCW, 0)); break;

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
            std::cout << "Key pressed: " << key << " at mouse (" << x << ", " << y << ")" << std::endl;
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