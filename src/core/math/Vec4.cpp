#include "Vec4.h"

Vec4 operator-(const Vec4 &a) {
    return Vec4(-a.x, -a.y, -a.z, -a.w);
}

Vec4 operator+(const Vec4 &a, const Vec4 &b) {
    return Vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

Vec4 operator-(const Vec4 &a, const Vec4 &b) {
    return Vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

Vec4 operator*(const Vec4 &a, const Vec4 &b) {
    return Vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

Vec4 operator/(const Vec4 &a, const Vec4 &b) {
    return Vec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

Vec4 operator*(const Vec4 &a, float b) {
    return Vec4(a.x * b, a.y * b, a.z * b, a.w * b);
}

Vec4 operator*(float a, const Vec4 &b) {
    return Vec4(a * b.x, a * b.y, a * b.z, a * b.w);
}

Vec4 operator/(const Vec4 &a, float b) {
    return Vec4(a.x / b, a.y / b, a.z / b, a.w / b);
}

Vec4 operator/(float a, const Vec4 &b) {
    return Vec4(a / b.x, a / b.y, a / b.z, a / b.w);
}

bool operator>(const Vec4 &a, const Vec4 &b) {
    return a.x > b.x && a.y > b.y && a.z > b.z && a.w > b.w;
}

bool operator<(const Vec4 &a, const Vec4 &b) {
    return a.x < b.x && a.y < b.y && a.z < b.z && a.w < b.w;
}

bool operator>=(const Vec4 &a, const Vec4 &b) {
    return a.x >= b.x && a.y >= b.y && a.z >= b.z && a.w >= b.w;
}

bool operator<=(const Vec4 &a, const Vec4 &b) {
    return a.x <= b.x && a.y <= b.y && a.z <= b.z && a.w <= b.w;
}

bool operator==(const Vec4 &a, const Vec4 &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

bool operator!=(const Vec4 &a, const Vec4 &b) {
    return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}

// Vec4 Exp(const Vec4 &v) {
//     return Vec4(Math::exp(v.x), Math::exp(v.y));
// }

// Vec4 Pow(const Vec4 &v, float p) {
//     return Vec4(Math::pow(v.x, p), Math::pow(v.y, p));
// }

// StringFormatter &operator<<(StringFormatter &f, const Vec4 &v) {
//     return f << "(" << v.x << ", " << v.y << ")";
// }
