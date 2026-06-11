#include "MathUtils.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline float degToRad(float deg) {
    return deg * static_cast<float>(M_PI) / 180.0f;
}

Mat4::Mat4() {
    // Default to identity
    for (int i = 0; i < 16; ++i) {
        m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }
}

Mat4 Mat4::identity() {
    return Mat4();
}

Mat4 Mat4::translation(float tx, float ty, float tz) {
    Mat4 res;
    res.m[12] = tx;
    res.m[13] = ty;
    res.m[14] = tz;
    return res;
}

Mat4 Mat4::scaling(float sx, float sy, float sz) {
    Mat4 res;
    res.m[0] = sx;
    res.m[5] = sy;
    res.m[10] = sz;
    return res;
}

Mat4 Mat4::rotationX(float angleDegrees) {
    Mat4 res;
    float rad = degToRad(angleDegrees);
    float c = std::cos(rad);
    float s = std::sin(rad);

    res.m[5] = c;
    res.m[6] = s;
    res.m[9] = -s;
    res.m[10] = c;

    return res;
}

Mat4 Mat4::rotationY(float angleDegrees) {
    Mat4 res;
    float rad = degToRad(angleDegrees);
    float c = std::cos(rad);
    float s = std::sin(rad);

    res.m[0] = c;
    res.m[2] = -s;
    res.m[8] = s;
    res.m[10] = c;

    return res;
}

Mat4 Mat4::rotationZ(float angleDegrees) {
    Mat4 res;
    float rad = degToRad(angleDegrees);
    float c = std::cos(rad);
    float s = std::sin(rad);

    res.m[0] = c;
    res.m[1] = s;
    res.m[4] = -s;
    res.m[5] = c;

    return res;
}

Mat4 Mat4::operator*(const Mat4& o) const {
    Mat4 res;
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += m[row + k * 4] * o.m[k + col * 4];
            }
            res.m[row + col * 4] = sum;
        }
    }
    return res;
}

Vec3 Mat4::operator*(const Vec3& v) const {
    // Multiplies as 3D coordinate point (w = 1)
    float x = m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12];
    float y = m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13];
    float z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14];
    float w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15];

    if (std::abs(w) > 1e-6f && std::abs(w - 1.0f) > 1e-6f) {
        return Vec3(x / w, y / w, z / w);
    }
    return Vec3(x, y, z);
}
