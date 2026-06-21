#include "Picking.h"
#include "../algorithms/Algorithms.h"
#include "../algorithms/Transformations.h"
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Private helper to draw a solid cubie base with a flat color
static void drawPickCube(float size) {
    float h = size * 0.5f;
    glBegin(GL_QUADS);
    // Right face
    glVertex3f( h, -h, -h);
    glVertex3f( h,  h, -h);
    glVertex3f( h,  h,  h);
    glVertex3f( h, -h,  h);

    // Left face
    glVertex3f(-h, -h,  h);
    glVertex3f(-h,  h,  h);
    glVertex3f(-h,  h, -h);
    glVertex3f(-h, -h, -h);

    // Up face
    glVertex3f(-h,  h,  h);
    glVertex3f( h,  h,  h);
    glVertex3f( h,  h, -h);
    glVertex3f(-h,  h, -h);

    // Down face
    glVertex3f(-h, -h, -h);
    glVertex3f( h, -h, -h);
    glVertex3f( h, -h,  h);
    glVertex3f(-h, -h,  h);

    // Front face
    glVertex3f(-h, -h,  h);
    glVertex3f( h, -h,  h);
    glVertex3f( h,  h,  h);
    glVertex3f(-h,  h,  h);

    // Back face
    glVertex3f( h, -h, -h);
    glVertex3f(-h, -h, -h);
    glVertex3f(-h,  h, -h);
    glVertex3f( h,  h, -h);
    glEnd();
}

bool pickCubie(int mouseX, int mouseY, const RubiksCube& cube, Camera& camera, 
               const Animation& animation, float explosionFactor, 
               int windowWidth, int windowHeight, 
               int& outIx, int& outIy, int& outIz) {
    
    // 1. Prepare OpenGL state for flat color rendering (no lighting, textures, shading)
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    
    // Clear back buffer with black (0,0,0) which indicates "no hit"
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 2. Set up projection matrix and camera view (exactly identical to normal display pass)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    float aspect = (float)windowWidth / (float)windowHeight;
    setupPerspectiveProjection(45.0, aspect, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    camera.apply();
    
    // 3. Render all cubies with encoded colors
    int size = cube.getSize();
    float halfSize = (size - 1) / 2.0f;
    const auto& cubies = cube.getCubies();
    
    for (const auto& cubie : cubies) {
        float tx = cubie.ix - halfSize;
        float ty = cubie.iy - halfSize;
        float tz = cubie.iz - halfSize;
        applyExplosionOffset(tx, ty, tz, explosionFactor);
        
        float rotAngle = 0.0f;
        float ax = 0.0f, ay = 0.0f, az = 0.0f;
        animation.getCubieRotation(cubie.ix, cubie.iy, cubie.iz, size,
                                   rotAngle, ax, ay, az);
        
        applyCubieTransform(rotAngle, ax, ay, az, tx, ty, tz);
        
        // Encode ix, iy, iz into RGB colors. Add 1 to avoid conflicts with background (0,0,0)
        // ix, iy, iz are in [0, 6] for sizes up to 7x7. This easily fits into [0, 255] color bytes.
        unsigned char r = cubie.ix + 1;
        unsigned char g = cubie.iy + 1;
        unsigned char b = cubie.iz + 1;
        
        glColor3ub(r, g, b);
        
        drawPickCube(0.96f);
        
        restoreTransform();
    }
    
    // 4. Read the pixel under the mouse cursor
    // GLUT coordinate has (0,0) at top-left. OpenGL has (0,0) at bottom-left.
    int glX = mouseX;
    int glY = windowHeight - mouseY;
    
    unsigned char pixel[3] = {0, 0, 0};
    glReadPixels(glX, glY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    
    // 5. Restore matrices
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW); // return to modelview
    
    // Decode the read color
    if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) {
        return false; // Background hit
    }
    
    outIx = pixel[0] - 1;
    outIy = pixel[1] - 1;
    outIz = pixel[2] - 1;
    
    // Sanity check coordinates are within bounds
    if (outIx < 0 || outIx >= size || 
        outIy < 0 || outIy >= size || 
        outIz < 0 || outIz >= size) {
        return false;
    }
    
    return true;
}
