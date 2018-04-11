#include "Mat4.h"

Mat4::Mat4() {
    a12 = a13 = a14 = 0.f;
    a21 = a23 = a24 = 0.f;
    a31 = a32 = a34 = 0.f;
    a41 = a42 = a43 = 0.f;
    a11 = a22 = a33 = a44 = 1.f;
}

Mat4::Mat4(
    float a11_, float a12_, float a13_, float a14_,
    float a21_, float a22_, float a23_, float a24_,
    float a31_, float a32_, float a33_, float a34_,
    float a41_, float a42_, float a43_, float a44_)
{
    a11 = a11_; a12 = a12_; a13 = a13_; a14 = a14_;
    a21 = a21_; a22 = a22_; a23 = a23_; a24 = a24_;
    a31 = a31_; a32 = a32_; a33 = a33_; a34 = a34_;
    a41 = a41_; a42 = a42_; a43 = a43_; a44 = a44_;
}

float Mat4::det() const {
    return +(a11*a22-a12*a21)*(a33*a44-a34*a43)
           -(a11*a23-a13*a21)*(a32*a44-a34*a42)
           +(a11*a24-a14*a21)*(a32*a43-a33*a42)
           +(a12*a23-a13*a22)*(a31*a44-a34*a41)
           -(a12*a24-a14*a22)*(a31*a43-a33*a41)
           +(a13*a24-a14*a23)*(a31*a42-a32*a41);
}

void Mat4::transpose() {
    *this = transposed();
}

Mat4 Mat4::transposed() const {
    return Mat4(
        a11, a21, a31, a41,
        a12, a22, a32, a42,
        a13, a23, a33, a43,
        a14, a24, a34, a44
    );
}

void Mat4::invert() {
    *this = inverted();
}

Mat4 Mat4::inverted() const {
    float d = det();
    // TODO check for zero

    float invDet = 1.f / d;
    return Mat4(invDet*(a22*(a33*a44-a34*a43)+a23*(a34*a42-a32*a44)+a24*(a32*a43-a33*a42)),
                invDet*(a32*(a13*a44-a14*a43)+a33*(a14*a42-a12*a44)+a34*(a12*a43-a13*a42)),
                invDet*(a42*(a13*a24-a14*a23)+a43*(a14*a22-a12*a24)+a44*(a12*a23-a13*a22)),
                invDet*(a12*(a24*a33-a23*a34)+a13*(a22*a34-a24*a32)+a14*(a23*a32-a22*a33)),
                invDet*(a23*(a31*a44-a34*a41)+a24*(a33*a41-a31*a43)+a21*(a34*a43-a33*a44)),
                invDet*(a33*(a11*a44-a14*a41)+a34*(a13*a41-a11*a43)+a31*(a14*a43-a13*a44)),
                invDet*(a43*(a11*a24-a14*a21)+a44*(a13*a21-a11*a23)+a41*(a14*a23-a13*a24)),
                invDet*(a13*(a24*a31-a21*a34)+a14*(a21*a33-a23*a31)+a11*(a23*a34-a24*a33)),
                invDet*(a24*(a31*a42-a32*a41)+a21*(a32*a44-a34*a42)+a22*(a34*a41-a31*a44)),
                invDet*(a34*(a11*a42-a12*a41)+a31*(a12*a44-a14*a42)+a32*(a14*a41-a11*a44)),
                invDet*(a44*(a11*a22-a12*a21)+a41*(a12*a24-a14*a22)+a42*(a14*a21-a11*a24)),
                invDet*(a14*(a22*a31-a21*a32)+a11*(a24*a32-a22*a34)+a12*(a21*a34-a24*a31)),
                invDet*(a21*(a33*a42-a32*a43)+a22*(a31*a43-a33*a41)+a23*(a32*a41-a31*a42)),
                invDet*(a31*(a13*a42-a12*a43)+a32*(a11*a43-a13*a41)+a33*(a12*a41-a11*a42)),
                invDet*(a41*(a13*a22-a12*a23)+a42*(a11*a23-a13*a21)+a43*(a12*a21-a11*a22)),
                invDet*(a11*(a22*a33-a23*a32)+a12*(a23*a31-a21*a33)+a13*(a21*a32-a22*a31)));
}

Mat4 Mat4::pseudoInverted() const {
    Mat4 trans = translate(Vec3(-a14, -a24, -a34));
    Mat4 rot = transposed();
    rot.a41 = rot.a42 = rot.a43 = 0.f;
    return rot * trans;
}

Mat3 Mat4::upper3x3() const {
    return Mat3(a11, a12, a13,
                a21, a22, a23,
                a31, a32, a33);
}

Mat4 Mat4::translate(const Vec3 &v) {
    return Mat4(
        1.f, 0.f, 0.f, v.x,
        0.f, 1.f, 0.f, v.y,
        0.f, 0.f, 1.f, v.z,
        0.f, 0.f, 0.f, 1.f
    );
}

Mat4 Mat4::scale(const Vec3 &s) {
    return Mat4(
        s.x, 0.f, 0.f, 0.f,
        0.f, s.y, 0.f, 0.f,
        0.f, 0.f, s.z, 0.f,
        0.f, 0.f, 0.f, 1.f
    );
}

Mat4 Mat4::rotXYZ(const Vec3 &rot) {
    float c[] = { std::cos(rot.x), std::cos(rot.y), std::cos(rot.z) };
    float s[] = { std::sin(rot.x), std::sin(rot.y), std::sin(rot.z) };

    return Mat4(
        c[1]*c[2], -c[0]*s[2] + s[0]*s[1]*c[2],  s[0]*s[2] + c[0]*s[1]*c[2], 0.f,
        c[1]*s[2],  c[0]*c[2] + s[0]*s[1]*s[2], -s[0]*c[2] + c[0]*s[1]*s[2], 0.f,
            -s[1],                   s[0]*c[1],                   c[0]*c[1], 0.f,
             0.f,                          0.f,                         0.f, 1.f
    );
}

Mat4 Mat4::rotYZX(const Vec3 &rot) {
    float c[] = { std::cos(rot.x), std::cos(rot.y), std::cos(rot.z) };
    float s[] = { std::sin(rot.x), std::sin(rot.y), std::sin(rot.z) };

    return Mat4(
         c[1]*c[2],  c[0]*c[1]*s[2] - s[0]*s[1], c[0]*s[1] + c[1]*s[0]*s[2], 0.f,
             -s[2],                   c[0]*c[2],                  c[2]*s[0], 0.f,
        -c[2]*s[1], -c[1]*s[0] - c[0]*s[1]*s[2], c[0]*c[1] - s[0]*s[1]*s[2], 0.f,
               0.f,                         0.f,                        0.f, 1.f
    );
}

Mat4 Mat4::rotAxis(const Vec3 &axis, float angle) {
    float s = std::sin(angle);
    float c = std::cos(angle);
    float c1 = 1.0f - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    return Mat4(
           c + x*x*c1,  x*y*c1 - z*s,  x*z*c1 + y*s, 0.f,
         y*x*c1 + z*s,    c + y*y*c1,  y*z*c1 - x*s, 0.f,
         z*x*c1 - y*s,  z*y*c1 + x*s,    c + z*z*c1, 0.f,
                  0.f,           0.f,           0.f, 1.f
    );
}

Mat4 Mat4::ortho(float l, float r, float b, float t, float n, float f) {
    return Mat4(
        2.f/(r-l),       0.f,        0.f, -(r+l)/(r-l),
              0.f, 2.f/(t-b),        0.f, -(t+b)/(t-b),
              0.f,       0.f, -2.f/(f-n), -(f+n)/(f-n),
              0.f,       0.f,        0.f,          1.f
    );
}

// fov in radians
Mat4 Mat4::perspective(float fov, float ratio, float nearZ, float farZ) {
    float t = 1.f / std::tan(0.5f * fov);
    float a = (farZ + nearZ)/(farZ - nearZ);
    float b = 2.f * farZ * nearZ / (farZ - nearZ);
    float c = t / ratio;

    return Mat4(
          c, 0.f,  0.f, 0.f,
        0.f,   t,  0.f, 0.f,
        0.f, 0.f,   -a,  -b,
        0.f, 0.f, -1.f, 0.f
    );
}

Mat4 Mat4::lookAt(const Vec3 &pos, const Vec3 &fwd, const Vec3 &up) {
    Vec3 f = fwd.normalized();
    Vec3 r = f.cross(up).normalized();
    Vec3 u = r.cross(f).normalized();

    return Mat4(
        r.x, u.x, -f.x, pos.x,
        r.y, u.y, -f.y, pos.y,
        r.z, u.z, -f.z, pos.z,
        0.f, 0.f, 0.f,   1.f
    );
}

Mat4 operator*(const Mat4 &a, const Mat4 &b) {
    Mat4 result;
    for (int i = 0; i < 4; ++i)
        for (int t = 0; t < 4; ++t)
            result.a[i*4 + t] =
                a.a[i*4 + 0]*b.a[0*4 + t] +
                a.a[i*4 + 1]*b.a[1*4 + t] +
                a.a[i*4 + 2]*b.a[2*4 + t] +
                a.a[i*4 + 3]*b.a[3*4 + t];

    return result;
}

Vec4 operator*(const Mat4 &a, const Vec4 &b) {
    return Vec4(
        a.a11*b.x + a.a12*b.y + a.a13*b.z + a.a14*b.w,
        a.a21*b.x + a.a22*b.y + a.a23*b.z + a.a24*b.w,
        a.a31*b.x + a.a32*b.y + a.a33*b.z + a.a34*b.w,
        a.a41*b.x + a.a42*b.y + a.a43*b.z + a.a44*b.w
    );
}

Vec3 operator*(const Mat4 &a, const Vec3 &b) {
    return Vec3(
        a.a11*b.x + a.a12*b.y + a.a13*b.z + a.a14,
        a.a21*b.x + a.a22*b.y + a.a23*b.z + a.a24,
        a.a31*b.x + a.a32*b.y + a.a33*b.z + a.a34
    );
}
