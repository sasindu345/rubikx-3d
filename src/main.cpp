#include <iostream>
#include "core/RubiksCube.h"
#include "core/CubeFactory.h"
#include "core/Move.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Screen dimensions
int windowWidth = 1024;
int windowHeight = 768;

// Display callback
void display() {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset transformations
    glLoadIdentity();
    
    // Basic draw placeholder (just a simple colored triangle to verify rendering works)
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); // Red
        glVertex2f(-0.5f, -0.5f);
        glColor3f(0.0f, 1.0f, 0.0f); // Green
        glVertex2f(0.5f, -0.5f);
        glColor3f(0.0f, 0.0f, 1.0f); // Blue
        glVertex2f(0.0f, 0.5f);
    glEnd();
    
    // Swap front and back buffers
    glutSwapBuffers();
}

// Reshape callback
void reshape(int width, int height) {
    if (height == 0) height = 1;
    
    windowWidth = width;
    windowHeight = height;
    
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspect = (float)width / (float)height;
    if (width >= height) {
        glOrtho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -1.0, 1.0);
    } else {
        glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -1.0, 1.0);
    }
    
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
        default:
            std::cout << "Key pressed: " << key << " at mouse (" << x << ", " << y << ")" << std::endl;
            break;
    }
}

// Special keyboard keys
void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            std::cout << "Left arrow pressed" << std::endl;
            break;
        case GLUT_KEY_RIGHT:
            std::cout << "Right arrow pressed" << std::endl;
            break;
        case GLUT_KEY_UP:
            std::cout << "Up arrow pressed" << std::endl;
            break;
        case GLUT_KEY_DOWN:
            std::cout << "Down arrow pressed" << std::endl;
            break;
    }
}

// Mouse button callbacks
void mouseButton(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        std::cout << "Mouse button " << button << " pressed at (" << x << ", " << y << ")" << std::endl;
    }
}

// Mouse motion callback
void mouseMotion(int x, int y) {}

// Initialize OpenGL state
void initGL() {
    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    
    std::cout << "OpenGL Initialized." << std::endl;
}

// Self-contained data model validation
void testCubeState() {
    std::cout << "\n=== Testing RubiksCube Data Model ===" << std::endl;
    RubiksCube cube = CubeFactory::create(3);
    std::cout << "Initial cube is solved: " << (cube.isSolved() ? "YES" : "NO") << std::endl;

    // Apply move R (Right face clockwise)
    Move rMove(Face::RIGHT, Direction::CW, 0);
    std::cout << "Applying move: " << rMove.toString() << std::endl;
    cube.applyMove(rMove);
    std::cout << "Cube is solved: " << (cube.isSolved() ? "YES" : "NO") << std::endl;

    // Apply inverse move R' (Right face counter-clockwise)
    Move rPrime = rMove.getInverse();
    std::cout << "Applying inverse move: " << rPrime.toString() << std::endl;
    cube.applyMove(rPrime);
    std::cout << "Cube is solved after inverse: " << (cube.isSolved() ? "YES" : "NO") << std::endl;

    // Apply multiple moves
    std::cout << "Applying scramble pattern U R' F..." << std::endl;
    cube.applyMove(Move(Face::UP, Direction::CW, 0));
    cube.applyMove(Move(Face::RIGHT, Direction::CCW, 0));
    cube.applyMove(Move(Face::FRONT, Direction::CW, 0));
    std::cout << "Cube is solved: " << (cube.isSolved() ? "YES" : "NO") << std::endl;

    std::cout << "Resetting cube..." << std::endl;
    cube.reset();
    std::cout << "Cube is solved after reset: " << (cube.isSolved() ? "YES" : "NO") << std::endl;
    std::cout << "=== RubiksCube Data Model Test Passed! ===\n" << std::endl;
}

int main(int argc, char** argv) {
    // Run tests
    testCubeState();

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
    
    glutMainLoop();
    
    return 0;
}
