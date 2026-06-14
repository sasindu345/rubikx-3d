/*
--------------------------------------------------
Module: Lighting System

Purpose:
Configures three-dimensional light sources and material reflection attributes
to render realistic plastic glossy surfaces for the cube.

Graphics Concepts:
- Illumination Models
- Key Light / Fill Light Positioning
- Material Specular and Shininess Reflections
- Color Tracking (GL_COLOR_MATERIAL)

Mathematics:
- Direction Vectors for Light Coordinates
- Diffuse, Specular, and Ambient Reflection Coefficients

Responsibilities:
- Initializing light parameters (GL_LIGHT0, GL_LIGHT1)
- Position lights dynamically within eye space
- Configuring material specular highlights
--------------------------------------------------
*/

#include "Lighting.h"

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
