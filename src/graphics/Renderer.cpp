#include "graphics/Renderer.h"
#include "graphics/Animation.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

Renderer::Renderer() {}
Renderer::~Renderer() {}

void Renderer::drawCubieBase(float size) {
    float h = size * 0.5f;

    // Use a deep charcoal black color for the cubie base to look premium
    Vec3 blackVec = Colors::getColorVec(Colors::BLACK);
    glColor3f(blackVec.x, blackVec.y, blackVec.z);

    glBegin(GL_QUADS);
    // Right face (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(h, -h, -h);
    glVertex3f(h,  h, -h);
    glVertex3f(h,  h,  h);
    glVertex3f(h, -h,  h);

    // Left face (-X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-h, -h,  h);
    glVertex3f(-h,  h,  h);
    glVertex3f(-h,  h, -h);
    glVertex3f(-h, -h, -h);

    // Up face (+Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-h,  h,  h);
    glVertex3f( h,  h,  h);
    glVertex3f( h,  h, -h);
    glVertex3f(-h,  h, -h);

    // Down face (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-h, -h, -h);
    glVertex3f( h, -h, -h);
    glVertex3f( h, -h,  h);
    glVertex3f(-h, -h,  h);

    // Front face (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-h, -h,  h);
    glVertex3f( h, -h,  h);
    glVertex3f( h,  h,  h);
    glVertex3f(-h,  h,  h);

    // Back face (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f( h, -h, -h);
    glVertex3f(-h, -h, -h);
    glVertex3f(-h,  h, -h);
    glVertex3f( h,  h, -h);
    glEnd();
}

void Renderer::drawFacelet(Face face, Colors::ColorName color) {
    if (color == Colors::NONE) return;

    Vec3 colorVec = Colors::getColorVec(color);
    glColor3f(colorVec.x, colorVec.y, colorVec.z);

    // offset is slightly larger than base box half-size to avoid z-fighting
    float offset = 0.49f; 
    float w = 0.43f; // sticker width (gives 0.05 margin)

    glBegin(GL_QUADS);
    switch (face) {
        case Face::RIGHT:
            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(offset, -w, -w);
            glVertex3f(offset,  w, -w);
            glVertex3f(offset,  w,  w);
            glVertex3f(offset, -w,  w);
            break;
        case Face::LEFT:
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glVertex3f(-offset, -w,  w);
            glVertex3f(-offset,  w,  w);
            glVertex3f(-offset,  w, -w);
            glVertex3f(-offset, -w, -w);
            break;
        case Face::UP:
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-w, offset,  w);
            glVertex3f( w, offset,  w);
            glVertex3f( w, offset, -w);
            glVertex3f(-w, offset, -w);
            break;
        case Face::DOWN:
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(-w, -offset, -w);
            glVertex3f( w, -offset, -w);
            glVertex3f( w, -offset,  w);
            glVertex3f(-w, -offset,  w);
            break;
        case Face::FRONT:
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-w, -w, offset);
            glVertex3f( w, -w, offset);
            glVertex3f( w,  w, offset);
            glVertex3f(-w,  w, offset);
            break;
        case Face::BACK:
            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3f( w, -w, -offset);
            glVertex3f(-w, -w, -offset);
            glVertex3f(-w,  w, -offset);
            glVertex3f( w,  w, -offset);
            break;
    }
    glEnd();
}

void Renderer::renderCube(const RubiksCube& cube, const Animation& animation) {
    int size = cube.getSize();
    float halfSize = (size - 1) / 2.0f;

    for (const auto& cubie : cube.getCubies()) {
        glPushMatrix();

        float tx = cubie.ix - halfSize;
        float ty = cubie.iy - halfSize;
        float tz = cubie.iz - halfSize;

        float rotAngle = 0.0f;
        float ax = 0.0f, ay = 0.0f, az = 0.0f;

        // If this cubie is part of the rotating slice in the active animation,
        // we rotate it about the cube center (origin) BEFORE translating.
        if (animation.getCubieRotation(cubie.ix, cubie.iy, cubie.iz, size, rotAngle, ax, ay, az)) {
            glRotatef(rotAngle, ax, ay, az);
        }

        glTranslatef(tx, ty, tz);

        // Render black background block
        drawCubieBase(0.96f);

        // Render colored sticker quads
        for (int f = 0; f < 6; ++f) {
            drawFacelet(static_cast<Face>(f), cubie.colors[f]);
        }

        glPopMatrix();
    }
}
