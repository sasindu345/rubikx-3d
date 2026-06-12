#include <iostream>
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

// Solution player
SolutionPlayer solutionPlayer;
HUD hud;
bool showHelp = true;

// Move history tracker
std::vector<Move> moveHistory;

void queueUserMove(const Move& m) {
    // Clear any active solver playback when user manually rotates a face
    solutionPlayer.setMoves({});
    
    // Maintain history (cancel adjacent inverses)
    if (!moveHistory.empty() && moveHistory.back() == m.getInverse()) {
        moveHistory.pop_back();
    } else {
        moveHistory.push_back(m);
    }
    
    animation.queueMove(m);
}

// Mouse tracking state
bool isLeftMouseDown = false;
bool isRightMouseDown = false;
int lastMouseX = 0;
int lastMouseY = 0;

// Display callback
void display() {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset modelview transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Position/orient the camera
    camera.apply();
    
    // Position lighting in scene space (after camera transformation so lights are stationary in world space)
    Lighting::apply();
    
    // Render the Rubik's Cube with active animations
    renderer.renderCube(activeCube, animation);
    
    // Render 2D HUD Help Menu overlay
    hud.render(windowWidth, windowHeight, solutionPlayer, showHelp);
    
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

// Keyboard callback
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // Escape key
            std::cout << "Exiting application." << std::endl;
            exit(0);
            break;
        
        // Toggle Help (H/h)
        case 'H':
        case 'h':
            showHelp = !showHelp;
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
        
        // Scramble
        case 'S':
        case 's':
            if (!animation.isAnimating() && animation.moveQueue.empty()) {
                std::cout << "Generating scramble..." << std::endl;
                activeCube.reset();
                moveHistory.clear();
                solutionPlayer.setMoves({});

                int steps = (activeCube.getSize() == 2) ? 10 : (activeCube.getSize() == 3) ? 20 : 25;
                std::vector<Move> moves = Scrambler::generateScramble(activeCube.getSize(), steps);
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
        
        // Switch Cube Size (3: 3x3, 4: 4x4, 5: 5x5)
        case '3':
            if (activeCube.getSize() != 3) {
                activeCube = CubeFactory::create(3);
                moveHistory.clear();
                solutionPlayer.setMoves({});
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
                camera.radius = 14.0f; // Zoom out further for 5x5 cube
                std::cout << "Switched to 5x5 Rubik's Cube." << std::endl;
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

        default:
            std::cout << "Key pressed: " << key << " at mouse (" << x << ", " << y << ")" << std::endl;
            break;
    }
}

// Special keyboard keys (arrow keys rotation test)
void specialKeys(int key, int x, int y) {
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

// Mouse button callbacks
void mouseButton(int button, int state, int x, int y) {
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
        }
    }

    // Update solution playback (if playing)
    solutionPlayer.update(deltaTime, animation, activeCube);

    // Reset scramble state and speed once scramble animation is finished
    if (isScrambling && !animation.isAnimating() && animation.moveQueue.empty()) {
        isScrambling = false;
        animation.speed = 450.0f; // Restore normal play speed
        std::cout << "Scramble complete." << std::endl;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // queue next frame
}

int main(int argc, char** argv) {
    // Run tests
    testCubeState();

    // Initialize default cube size 3x3
    activeCube = CubeFactory::create(3);

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("RubikX-3D - Learning & Solving System");
    
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
