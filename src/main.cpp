#include <iostream>

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
    // Prevent division by zero
    if (height == 0) height = 1;
    
    windowWidth = width;
    windowHeight = height;
    
    // Set viewport to cover the new window size
    glViewport(0, 0, width, height);
    
    // Set projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Setup simple 2D orthographic projection for the initial phase
    float aspect = (float)width / (float)height;
    if (width >= height) {
        glOrtho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -1.0, 1.0);
    } else {
        glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -1.0, 1.0);
    }
    
    // Set modelview matrix
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

// Special keyboard keys (e.g. arrow keys)
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

// Mouse motion (active drag) callback
void mouseMotion(int x, int y) {
    // Placeholder for dragging behavior
}

// Initialize OpenGL state
void initGL() {
    // Set clear color to a sleek dark mode theme
    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    
    // Smooth shading
    glShadeModel(GL_SMOOTH);
    
    std::cout << "OpenGL Initialized." << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
}

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    
    // Set display mode (Double buffered, RGB color, Depth buffer)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    
    // Set initial window size
    glutInitWindowSize(windowWidth, windowHeight);
    
    // Set initial window position
    glutInitWindowPosition(100, 100);
    
    // Create window
    glutCreateWindow("RubikX-3D - Learning & Solving System");
    
    // Initialize OpenGL functions
    initGL();
    
    // Register callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    
    // Enter the main execution loop
    glutMainLoop();
    
    return 0;
}
