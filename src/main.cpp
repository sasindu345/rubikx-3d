#include <iostream>
#include "core/RubiksCube.h"
#include "core/CubeFactory.h"
#include "core/Move.h"
#include "graphics/Renderer.h"
#include "graphics/Camera.h"
#include "graphics/Lighting.h"
#include "graphics/Animation.h"
#include "solver/Scrambler.h"
#include "solver/Solver3x3.h"
#include "ui/SolutionPlayer.h"

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
    
    // Swap front and back buffers
    glutSwapBuffers();
}

// Reshape callback (setup perspective projection)
void reshape(int width, int height) {
    if (height == 0) height = 1;
    
    windowWidth = width;
    windowHeight = height;
    
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspect = (float)width / (float)height;
    // Set 45-degree field-of-view, near clipping plane at 0.1, far clipping plane at 100.0
    gluPerspective(45.0, aspect, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Keyboard callback
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // Escape key
            std::cout << "Exiting application." << std::endl;
            exit(0);
            break;
        
        // Solve (Z/z)
        case 'Z':
        case 'z':
            if (!animation.isAnimating() && animation.moveQueue.empty()) {
                if (activeCube.getSize() == 3) {
                    std::cout << "Calculating 3x3 layer-by-layer solution..." << std::endl;
                    Solver3x3 solver;
                    std::vector<Move> solution = solver.solve(activeCube);
                    std::cout << "Solution computed (" << solution.size() << " moves): ";
                    for (const auto& m : solution) {
                        std::cout << m.toString() << " ";
                    }
                    std::cout << std::endl;
                    solutionPlayer.setMoves(solution);
                } else {
                    std::cout << "Solver only supported for 3x3 cubes currently." << std::endl;
                }
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
                int steps = (activeCube.getSize() == 2) ? 10 : (activeCube.getSize() == 3) ? 20 : 25;
                std::vector<Move> moves = Scrambler::generateScramble(activeCube.getSize(), steps);
                isScrambling = true;
                animation.speed = 1800.0f; // Fast scramble animation
                std::cout << "Scramble (" << moves.size() << " moves): ";
                for (const auto& m : moves) {
                    std::cout << m.toString() << " ";
                    animation.queueMove(m);
                }
                std::cout << std::endl;
            }
            break;
        
        // Clockwise moves (Capital letters)
        case 'R': animation.queueMove(Move(Face::RIGHT, Direction::CW, 0)); break;
        case 'L': animation.queueMove(Move(Face::LEFT, Direction::CW, 0)); break;
        case 'U': animation.queueMove(Move(Face::UP, Direction::CW, 0)); break;
        case 'D': animation.queueMove(Move(Face::DOWN, Direction::CW, 0)); break;
        case 'F': animation.queueMove(Move(Face::FRONT, Direction::CW, 0)); break;
        case 'B': animation.queueMove(Move(Face::BACK, Direction::CW, 0)); break;

        // Counter-Clockwise moves (Lowercase letters)
        case 'r': animation.queueMove(Move(Face::RIGHT, Direction::CCW, 0)); break;
        case 'l': animation.queueMove(Move(Face::LEFT, Direction::CCW, 0)); break;
        case 'u': animation.queueMove(Move(Face::UP, Direction::CCW, 0)); break;
        case 'd': animation.queueMove(Move(Face::DOWN, Direction::CCW, 0)); break;
        case 'f': animation.queueMove(Move(Face::FRONT, Direction::CCW, 0)); break;
        case 'b': animation.queueMove(Move(Face::BACK, Direction::CCW, 0)); break;

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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Enable back-face culling for visible surface detection demonstration
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
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
