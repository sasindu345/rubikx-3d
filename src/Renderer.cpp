#include "Renderer.h"
#include "Animation.h"
#include "algorithms/Transformations.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
#include <algorithm>
#include <cmath>

// ============================================================
// CubieTransformData
// Holds pre-computed per-cubie transform so we can re-apply
// it in both the opaque and transparent render passes.
// ============================================================
struct CubieTransformData {
    float tx, ty, tz;          // translation (world position)
    float rotAngle;            // animation rotation angle (degrees)
    float ax, ay, az;          // animation rotation axis
};

Renderer::Renderer() {}
Renderer::~Renderer() {}

void Renderer::initTexture() {
    if (textureId != 0) return;
    
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    #ifdef GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    #else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    #endif
    
    // Generate a procedural texture (rounded corner sticker shape)
    const int texSize = 64;
    GLubyte texData[texSize][texSize][4];
    for (int y = 0; y < texSize; ++y) {
        for (int x = 0; x < texSize; ++x) {
            // Normalize coordinates to [-1, 1]
            float nx = (x - texSize / 2.0f) / (texSize / 2.0f);
            float ny = (y - texSize / 2.0f) / (texSize / 2.0f);
            
            // Rounded corners formula: x^4 + y^4 = r^4.
            float dist = std::pow(nx, 4.0f) + std::pow(ny, 4.0f);
            
            GLubyte r = 255, g = 255, b = 255, a = 255;
            
            if (dist > 1.0f) {
                // Outside the sticker corner - make transparent
                r = 0; g = 0; b = 0; a = 0;
            } else if (dist > 0.7f) {
                // Sticker border/edge - dark gray/black border
                r = 20; g = 20; b = 20; a = 255;
            } else {
                // Sticker body: nice subtle radial gradient
                float innerDist = std::sqrt(nx*nx + ny*ny);
                GLubyte val = (GLubyte)(255 - innerDist * 60); // subtle gradient from 255 to 195
                r = val; g = val; b = val; a = 255;
            }
            
            texData[y][x][0] = r;
            texData[y][x][1] = g;
            texData[y][x][2] = b;
            texData[y][x][3] = a;
        }
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
}

// -----------------------------------------------------------
// drawCubieBase
// Draws the solid black structural cube for each cubie.
// When alpha < 1.0 (glass mode), faces are rendered with
// transparency via GL_BLEND (alpha compositing theory).
// -----------------------------------------------------------
void Renderer::drawCubieBase(float size, float alpha) {
    float h = size * 0.5f;

    // Use glColor4f so alpha is forwarded to the blending pipeline.
    // GL_COLOR_MATERIAL ensures this feeds into the lighting material.
    if (renderMode == RenderMode::WIREFRAME) {
        // Draw in dark gray-blue cage so wireframe base is visible on the dark background
        glColor4f(0.25f, 0.25f, 0.3f, alpha);
    } else {
        Vec3 blackVec = Colors::getColorVec(Colors::BLACK);
        glColor4f(blackVec.x, blackVec.y, blackVec.z, alpha);
    }

    glBegin(GL_QUADS);
    // Right face (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f( h, -h, -h);
    glVertex3f( h,  h, -h);
    glVertex3f( h,  h,  h);
    glVertex3f( h, -h,  h);

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

// -----------------------------------------------------------
// drawFacelet
// Draws a single colored sticker quad on the given face.
// alpha is passed via glColor4f for blending support.
// -----------------------------------------------------------
void Renderer::drawFacelet(Face face, Colors::ColorName color, float alpha) {
    if (color == Colors::NONE) return;

    bool textured = (renderMode == RenderMode::TEXTURED && textureId != 0);

    Vec3 colorVec = Colors::getColorVec(color);
    glColor4f(colorVec.x, colorVec.y, colorVec.z, alpha);

    if (textured) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // offset is slightly larger than base box half-size to avoid z-fighting
    float offset = 0.49f;
    float w = 0.43f; // sticker width (gives 0.05 margin)

    glBegin(GL_QUADS);
    switch (face) {
        case Face::RIGHT:
            glNormal3f(1.0f, 0.0f, 0.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(offset, -w, -w);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(offset,  w, -w);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(offset,  w,  w);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(offset, -w,  w);
            break;
        case Face::LEFT:
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-offset, -w,  w);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(-offset,  w,  w);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(-offset,  w, -w);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-offset, -w, -w);
            break;
        case Face::UP:
            glNormal3f(0.0f, 1.0f, 0.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, offset,  w);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( w, offset,  w);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( w, offset, -w);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, offset, -w);
            break;
        case Face::DOWN:
            glNormal3f(0.0f, -1.0f, 0.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -offset, -w);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( w, -offset, -w);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( w, -offset,  w);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, -offset,  w);
            break;
        case Face::FRONT:
            glNormal3f(0.0f, 0.0f, 1.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -w, offset);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( w, -w, offset);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( w,  w, offset);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-w,  w, offset);
            break;
        case Face::BACK:
            glNormal3f(0.0f, 0.0f, -1.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f( w, -w, -offset);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -w, -offset);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(-w,  w, -offset);
            glTexCoord2f(0.0f, 1.0f); glVertex3f( w,  w, -offset);
            break;
    }
    glEnd();

    if (textured) {
        glDisable(GL_TEXTURE_2D);
        if (!alphaBlending) {
            glDisable(GL_BLEND);
        }
    }
}

// -----------------------------------------------------------
// applyCubieRenderTransform (private helper)
// Pushes matrix and applies animation + translation so the
// same transform is reproducible in both render passes.
// -----------------------------------------------------------
void Renderer::applyCubieRenderTransform(const CubieTransformData& d) {
    applyCubieTransform(d.rotAngle, d.ax, d.ay, d.az, d.tx, d.ty, d.tz);
}

// -----------------------------------------------------------
// renderCube
//
// NORMAL mode  : single-pass, fully opaque rendering.
//
// ALPHA BLENDING mode (Glass Cube):
//   Theory: Alpha compositing — C_out = C_src * A_src + C_dst * (1 - A_src)
//   Using:  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
//
//   Two-pass strategy for correct transparency:
//   Pass 1 — Opaque stickers (all cubies, depth write ON, blend OFF)
//             Stickers are always fully opaque so they write correct
//             depth values before any transparent geometry.
//   Pass 2 — Transparent bases (back-to-front Painter's Algorithm,
//             depth write OFF, blend ON, alpha ~0.35)
//             Sorting prevents occlusion artefacts between transparent faces.
// -----------------------------------------------------------
void Renderer::renderCube(const RubiksCube& cube, const Animation& animation) {
    if (renderMode == RenderMode::TEXTURED) {
        initTexture();
    }

    if (renderMode == RenderMode::WIREFRAME) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_LIGHTING);
    }

    int size = cube.getSize();
    float halfSize = (size - 1) / 2.0f;

    // --------------------------------------------------------
    // Build per-cubie transform data list (computed once)
    // --------------------------------------------------------
    const auto& cubies = cube.getCubies();
    std::vector<CubieTransformData> transforms;
    transforms.reserve(cubies.size());

    for (const auto& cubie : cubies) {
        CubieTransformData d;
        d.tx = cubie.ix - halfSize;
        d.ty = cubie.iy - halfSize;
        d.tz = cubie.iz - halfSize;

        // ── Exploded View Transformation ─────────────────────
        // Push each cubie outward from the cube centre along its
        // radial direction vector.  The displacement magnitude is
        // proportional to explosionFactor (0 = compact, 1 = fully
        // expanded).  We scale by the cubie's distance from center
        // so corner pieces move further than edge/center pieces.
        //
        // Formula:  T' = T + explosionFactor * T  (component-wise)
        //         = T * (1 + explosionFactor)
        if (explosionFactor > 0.001f) {
            d.tx *= (1.0f + explosionFactor);
            d.ty *= (1.0f + explosionFactor);
            d.tz *= (1.0f + explosionFactor);
        }

        d.rotAngle = 0.0f;
        d.ax = d.ay = d.az = 0.0f;
        animation.getCubieRotation(cubie.ix, cubie.iy, cubie.iz, size,
                                   d.rotAngle, d.ax, d.ay, d.az);
        transforms.push_back(d);
    }

    // ========================================================
    // NORMAL MODE  — single-pass opaque rendering
    // ========================================================
    if (!alphaBlending) {
        for (size_t i = 0; i < cubies.size(); ++i) {
            applyCubieRenderTransform(transforms[i]);

            drawCubieBase(0.96f, 1.0f);   // fully opaque black base
            for (int f = 0; f < 6; ++f)
                drawFacelet(static_cast<Face>(f), cubies[i].colors[f], 1.0f);

            restoreTransform();
        }
        
        // Restore OpenGL state to defaults
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_LIGHTING);
        return;
    }

    // ========================================================
    // ALPHA BLENDING MODE — Glass Cube
    // ========================================================

    // ----------------------------------------------------------
    // PASS 1: Opaque stickers only
    // Depth writes ON so stickers correctly occlude what is behind.
    // Blending OFF — stickers are 100% opaque solid colour.
    // ----------------------------------------------------------
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    for (size_t i = 0; i < cubies.size(); ++i) {
        applyCubieRenderTransform(transforms[i]);
        for (int f = 0; f < 6; ++f)
            drawFacelet(static_cast<Face>(f), cubies[i].colors[f], 1.0f);
        restoreTransform();
    }

    // ----------------------------------------------------------
    // PASS 2: Semi-transparent cubie bases (glass shells)
    // Sort cubies back-to-front relative to camera (Painter's Algorithm)
    // so that farther cubie bases blend onto nearer ones correctly.
    // Depth writes OFF so transparent faces don't occlude each other.
    // ----------------------------------------------------------

    // Retrieve the current modelview matrix to get camera-space Z depth.
    // We use the Z component of the modelview-transformed position as sort key.
    float mv[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mv);

    // Build sortable index list with camera-space depth
    std::vector<std::pair<float, size_t>> depthIndex;
    depthIndex.reserve(cubies.size());
    for (size_t i = 0; i < cubies.size(); ++i) {
        const CubieTransformData& d = transforms[i];
        // Transform cubie centre through the current modelview (camera transform).
        // For sorting we only need approximate depth; use un-animated position
        // (good enough — animation offsets are small relative to sort order).
        float cx = mv[0]*d.tx + mv[4]*d.ty + mv[8]*d.tz  + mv[12];
        float cy = mv[1]*d.tx + mv[5]*d.ty + mv[9]*d.tz  + mv[13];
        float cz = mv[2]*d.tx + mv[6]*d.ty + mv[10]*d.tz + mv[14];
        (void)cx; (void)cy;
        depthIndex.push_back({ cz, i });   // smaller (more negative) = farther away
    }

    // Sort ascending by cz  →  farthest first  (Painter's Algorithm: draw far-to-near)
    std::sort(depthIndex.begin(), depthIndex.end(),
              [](const std::pair<float,size_t>& a, const std::pair<float,size_t>& b) {
                  return a.first < b.first;
              });

    // Enable alpha blending: standard over-compositing
    //   C_out = C_src * A_src  +  C_dst * (1 - A_src)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth writes so transparent shells don't block each other
    glDepthMask(GL_FALSE);

    const float GLASS_ALPHA = 0.30f;  // 30% opaque — tweak for desired glass thickness

    for (const auto& [depth, i] : depthIndex) {
        applyCubieRenderTransform(transforms[i]);
        drawCubieBase(0.96f, GLASS_ALPHA);
        restoreTransform();
    }

    // Restore OpenGL state to defaults
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_LIGHTING);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
