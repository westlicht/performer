#pragma once

#include "Vec3.h"
#include "Vec4.h"
#include "Mat3.h"

struct Mat4 {
    Mat4();
    Mat4(float a11, float a12, float a13, float a14,
         float a21, float a22, float a23, float a24,
         float a31, float a32, float a33, float a34,
         float a41, float a42, float a43, float a44);

    float det() const;

    void transpose();
    Mat4 transposed() const;
    void invert();
    Mat4 inverted() const;
    Mat4 pseudoInverted() const;

    Mat3 upper3x3() const;

    static Mat4 translate(const Vec3 &v);
    static Mat4 scale(const Vec3 &s);
    static Mat4 rotXYZ(const Vec3 &rot);
    static Mat4 rotYZX(const Vec3 &rot);
    static Mat4 rotAxis(const Vec3 &axis, float angle);

    static Mat4 ortho(float l, float r, float b, float t, float nearZ, float farZ);
    static Mat4 perspective(float fov, float ratio, float nearZ, float farZ);
    static Mat4 lookAt(const Vec3 &pos, const Vec3 &fwd, const Vec3 &up);

    union {
         struct {
             float a11, a12, a13, a14;
             float a21, a22, a23, a24;
             float a31, a32, a33, a34;
             float a41, a42, a43, a44;
         };
         float a[16];
     };
};

Mat4 operator*(const Mat4 &a, const Mat4 &b);
Vec4 operator*(const Mat4 &a, const Vec4 &b);
Vec3 operator*(const Mat4 &a, const Vec3 &b);
