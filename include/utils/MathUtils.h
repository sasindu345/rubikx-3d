#pragma once

#include <cmath>
#include <iostream>

struct Vec3 {
    float x, y, z;

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Vector operations
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    Vec3 operator/(float s) const { return Vec3(x / s, y / s, z / s); }

    Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3& operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    Vec3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }

    bool operator==(const Vec3& v) const {
        const float eps = 1e-5f;
        return std::abs(x - v.x) < eps && std::abs(y - v.y) < eps && std::abs(z - v.z) < eps;
    }

    bool operator!=(const Vec3& v) const {
        return !(*this == v);
    }

    float dot(const Vec3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vec3 cross(const Vec3& v) const {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    float lengthSq() const {
        return x * x + y * y + z * z;
    }

    float length() const {
        return std::sqrt(lengthSq());
    }

    Vec3 normalized() const {
        float len = length();
        if (len < 1e-6f) return Vec3(0, 0, 0);
        return *this / len;
    }

    void normalize() {
        float len = length();
        if (len > 1e-6f) {
            *this /= len;
        }
    }
};

// Matrix 4x4 representation in COLUMN-MAJOR order (compatible with OpenGL)
struct Mat4 {
    float m[16];

    Mat4(); // Identity matrix constructor

    static Mat4 identity();
    static Mat4 translation(float tx, float ty, float tz);
    static Mat4 scaling(float sx, float sy, float sz);
    static Mat4 rotationX(float angleDegrees);
    static Mat4 rotationY(float angleDegrees);
    static Mat4 rotationZ(float angleDegrees);

    Mat4 operator*(const Mat4& o) const;
    Vec3 operator*(const Vec3& v) const; // Multiplies as point (w=1) or vector (w=0)? Typically point (w=1)
};
