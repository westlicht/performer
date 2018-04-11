#include "Mat3.h"

#include <algorithm>

Mat3::Mat3() {
    a12 = a13 = 0.f;
    a21 = a23 = 0.f;
    a31 = a32 = 0.f;
    a11 = a22 = a33 = 1.f;
}

Mat3::Mat3(
    float a11_, float a12_, float a13_,
    float a21_, float a22_, float a23_,
    float a31_, float a32_, float a33_)
{
    a11 = a11_; a12 = a12_; a13 = a13_;
    a21 = a21_; a22 = a22_; a23 = a23_;
    a31 = a31_; a32 = a32_; a33 = a33_;
}

float Mat3::det() const {
    return a11*a22*a33 + a12*a23*a31 + a13*a21*a32 - a13*a22*a31 - a12*a21*a33 - a11*a23*a32;
}

void Mat3::invert() {
    *this = inverted();
}

Mat3 Mat3::inverted() const {
    float d = det();
    // TODO check for zero

    float invd = 1.f / d;

    return Mat3((a22*a33 - a23*a32) * invd,
                (a13*a32 - a12*a33) * invd,
                (a12*a23 - a13*a22) * invd,
                (a23*a31 - a21*a33) * invd,
                (a11*a33 - a13*a31) * invd,
                (a13*a21 - a11*a23) * invd,
                (a21*a32 - a22*a31) * invd,
                (a12*a31 - a11*a32) * invd,
                (a11*a22 - a12*a21) * invd);
}

void Mat3::transpose() {
    std::swap(a12, a21);
    std::swap(a13, a31);
    std::swap(a23, a32);
}

Mat3 Mat3::transposed() const {
    Mat3 m = *this;
    m.transpose();
    return m;
}

Mat3 Mat3::scale(float scale) {
    return Mat3(
        scale, 0.f, 0.f,
        0.f, scale, 0.f,
        0.f, 0.f, scale
    );
}

Mat3 Mat3::rotateX(float theta) {
    float cosTheta = std::cos(theta);
    float sinTheta = std::sin(theta);
    return Mat3(
        1.f, 0.f, 0.f,
        0.f, cosTheta, -sinTheta,
        0.f, sinTheta, cosTheta
    );
}

Mat3 Mat3::rotateY(float theta) {
    float cosTheta = std::cos(theta);
    float sinTheta = std::sin(theta);
    return Mat3(
        cosTheta, 0.f, sinTheta,
        0.f, 1.f, 0.f,
        -sinTheta, 0.f, cosTheta
    );
}

Mat3 Mat3::rotateZ(float theta) {
    float cosTheta = std::cos(theta);
    float sinTheta = std::sin(theta);
    return Mat3(
        cosTheta, -sinTheta, 0.f,
        sinTheta, cosTheta, 0.f,
        0.f, 0.f, 1.f
    );
}

Mat3 Mat3::transform2D(const Vec2 &translate, float rotate, float scale) {
    float sinTheta = std::sin(rotate);
    float cosTheta = std::cos(rotate);
    return Mat3(
        cosTheta * scale, -sinTheta * scale, translate.x,
        sinTheta * scale,  cosTheta * scale, translate.y,
        0.f, 0.f, 0.f
    );
}

Mat3 operator*(const Mat3 &a, const Mat3 &b) {
    Mat3 result;
    for (int i = 0; i < 3; ++i)
        for (int t = 0; t < 3; ++t)
            result.a[i*3 + t] =
                a.a[i*3 + 0]*b.a[0*3 + t] +
                a.a[i*3 + 1]*b.a[1*3 + t] +
                a.a[i*3 + 2]*b.a[2*3 + t];

    return result;
}

Vec2 operator*(const Mat3 &a, const Vec2 &b) {
    return Vec2(
        a.a11*b.x + a.a12*b.y + a.a13,
        a.a21*b.x + a.a22*b.y + a.a23
    );
}


// Vec3 operator*(const Mat3 &a, const Vec3 &b)
// {
//     return Vec3(
//         a.a11*b.x + a.a12*b.y + a.a13*b.z,
//         a.a21*b.x + a.a22*b.y + a.a23*b.z,
//         a.a31*b.x + a.a32*b.y + a.a33*b.z
//     );
// }

// StringFormatter &operator<<(StringFormatter &f, const Mat3 &m)
// {
//     return f << "| " << m.a11 << ", " << m.a12 << ", " << m.a13 << " |\n"
//              << "| " << m.a21 << ", " << m.a22 << ", " << m.a23 << " |\n"
//              << "| " << m.a31 << ", " << m.a32 << ", " << m.a33 << " |\n";
// }
