#include "Vec3.h"

Vec3 operator-(const Vec3 &a) {
    return Vec3(-a.x, -a.y, -a.z);
}

Vec3 operator+(const Vec3 &a, const Vec3 &b) {
    return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 operator-(const Vec3 &a, const Vec3 &b) {
    return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 operator*(const Vec3 &a, const Vec3 &b) {
    return Vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vec3 operator/(const Vec3 &a, const Vec3 &b) {
    return Vec3(a.x / b.x, a.y / b.y, a.z / b.z);
}

Vec3 operator*(const Vec3 &a, float b) {
    return Vec3(a.x * b, a.y * b, a.z * b);
}

Vec3 operator*(float a, const Vec3 &b) {
    return Vec3(a * b.x, a * b.y, a * b.z);
}

Vec3 operator/(const Vec3 &a, float b) {
    return Vec3(a.x / b, a.y / b, a.z / b);
}

Vec3 operator/(float a, const Vec3 &b) {
    return Vec3(a / b.x, a / b.y, a / b.z);
}

bool operator>(const Vec3 &a, const Vec3 &b) {
    return a.x > b.x && a.y > b.y && a.z > b.z;
}

bool operator<(const Vec3 &a, const Vec3 &b) {
    return a.x < b.x && a.y < b.y && a.z < b.z;
}

bool operator>=(const Vec3 &a, const Vec3 &b) {
    return a.x >= b.x && a.y >= b.y && a.z >= b.z;
}

bool operator<=(const Vec3 &a, const Vec3 &b) {
    return a.x <= b.x && a.y <= b.y && a.z <= b.z;
}

bool operator==(const Vec3 &a, const Vec3 &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool operator!=(const Vec3 &a, const Vec3 &b) {
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

// Vec3 Exp(const Vec3 &v) {
//     return Vec3(Math::exp(v.x), Math::exp(v.y));
// }

// Vec3 Pow(const Vec3 &v, float p) {
//     return Vec3(Math::pow(v.x, p), Math::pow(v.y, p));
// }

// StringFormatter &operator<<(StringFormatter &f, const Vec3 &v) {
//     return f << "(" << v.x << ", " << v.y << ")";
// }
