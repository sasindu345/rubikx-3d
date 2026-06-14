/*
--------------------------------------------------
Module: Lighting System

Theory: Shading & Lighting
Lecture: Visible Surface Detection and Shading
Algorithm: Phong Illumination Model (Diffuse + Specular)
Mathematics:
  I = Ia * Ka  +  Id * Kd * (N . L)  +  Is * Ks * (R . V)^n
  where Ia/Id/Is = ambient/diffuse/specular light intensity
        Ka/Kd/Ks = material ambient/diffuse/specular coefficients
        N = surface normal, L = light direction, V = view direction
        R = reflected light direction, n = shininess exponent
Used by: Renderer.cpp, main.cpp
User Result: Realistic face reflections with dynamic orbiting light source.
Responsible Member: Member 2
--------------------------------------------------
*/

#include "Lighting.h"
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void Lighting::init() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    // Primary key light (warm, strong)
    float ambient0[] = { 0.15f, 0.15f, 0.15f, 1.0f };
    float diffuse0[] = { 0.85f, 0.85f, 0.80f, 1.0f };
    float specular0[] = { 0.40f, 0.40f, 0.40f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

    // Secondary fill light (cool, softer)
    float ambient1[] = { 0.05f, 0.05f, 0.08f, 1.0f };
    float diffuse1[] = { 0.25f, 0.25f, 0.35f, 1.0f };
    float specular1[] = { 0.10f, 0.10f, 0.15f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);

    // Global ambient illumination
    float globalAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    // Enable color tracking for materials (allows glColor to set ambient & diffuse)
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Material properties (glossy plastic appearance)
    float matSpecular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    float matShininess[] = { 64.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

void Lighting::apply() {
    // Key light positioned at top-right-front
    float lightPos0[] = { 6.0f, 10.0f, 8.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    // Fill light positioned at bottom-left-back
    float lightPos1[] = { -7.0f, -5.0f, -6.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
}

void Lighting::apply(float orbitAngleDeg, float orbitRadius) {
    // --------------------------------------------------------
    // Orbiting key light (GL_LIGHT0):
    // The light source orbits around the Y axis at a fixed
    // elevation, producing dynamically changing specular
    // highlights on the cube faces.
    //
    // Parametric circle:
    //   x = r * sin(theta)
    //   y = fixed elevation
    //   z = r * cos(theta)
    // --------------------------------------------------------
    float rad = orbitAngleDeg * 3.14159265f / 180.0f;
    float lx = orbitRadius * std::sin(rad);
    float ly = 8.0f; // Slightly elevated
    float lz = orbitRadius * std::cos(rad);

    float lightPos0[] = { lx, ly, lz, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    // Fill light stays in a fixed position (bottom-left-back)
    float lightPos1[] = { -7.0f, -5.0f, -6.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
}

void Lighting::renderLightIndicator(float orbitAngleDeg, float orbitRadius) {
    // --------------------------------------------------------
    // Renders a small glowing wireframe sphere at the position
    // of the orbiting key light so the user can see it.
    // Lighting is temporarily disabled so the indicator uses
    // a constant emissive colour unaffected by scene shading.
    // --------------------------------------------------------
    float rad = orbitAngleDeg * 3.14159265f / 180.0f;
    float lx = orbitRadius * std::sin(rad);
    float ly = 8.0f;
    float lz = orbitRadius * std::cos(rad);

    glDisable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(lx, ly, lz);

    // Bright yellow-white emissive colour for the light bulb
    glColor3f(1.0f, 0.95f, 0.6f);
    glutWireSphere(0.25, 12, 8);

    // Draw a subtle outer glow ring (slightly larger, dimmer)
    glColor4f(1.0f, 0.85f, 0.3f, 0.3f);
    glutWireSphere(0.40, 8, 6);

    glPopMatrix();

    glEnable(GL_LIGHTING);
}
