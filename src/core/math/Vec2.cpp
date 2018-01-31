#include "Vec2.h"

Vec2 operator-(const Vec2 &a) {
    return Vec2(-a.x, -a.y);
}

Vec2 operator+(const Vec2 &a, const Vec2 &b) {
    return Vec2(a.x + b.x, a.y + b.y);
}

Vec2 operator-(const Vec2 &a, const Vec2 &b) {
    return Vec2(a.x - b.x, a.y - b.y);
}

Vec2 operator*(const Vec2 &a, const Vec2 &b) {
    return Vec2(a.x * b.x, a.y * b.y);
}

Vec2 operator/(const Vec2 &a, const Vec2 &b) {
    return Vec2(a.x / b.x, a.y / b.y);
}

Vec2 operator*(const Vec2 &a, float b) {
    return Vec2(a.x * b, a.y * b);
}

Vec2 operator*(float a, const Vec2 &b) {
    return Vec2(a * b.x, a * b.y);
}

Vec2 operator/(const Vec2 &a, float b) {
    return Vec2(a.x / b, a.y / b);
}

Vec2 operator/(float a, const Vec2 &b) {
    return Vec2(a / b.x, a / b.y);
}

bool operator>(const Vec2 &a, const Vec2 &b) {
    return a.x > b.x && a.y > b.y;
}

bool operator<(const Vec2 &a, const Vec2 &b) {
    return a.x < b.x && a.y < b.y;
}

bool operator>=(const Vec2 &a, const Vec2 &b) {
    return a.x >= b.x && a.y >= b.y;
}

bool operator<=(const Vec2 &a, const Vec2 &b) {
    return a.x <= b.x && a.y <= b.y;
}

bool operator==(const Vec2 &a, const Vec2 &b) {
    return a.x == b.x && a.y == b.y;
}

bool operator!=(const Vec2 &a, const Vec2 &b) {
    return a.x != b.x || a.y != b.y;
}

// Vec2 Exp(const Vec2 &v) {
//     return Vec2(Math::exp(v.x), Math::exp(v.y));
// }

// Vec2 Pow(const Vec2 &v, float p) {
//     return Vec2(Math::pow(v.x, p), Math::pow(v.y, p));
// }

// StringFormatter &operator<<(StringFormatter &f, const Vec2 &v) {
//     return f << "(" << v.x << ", " << v.y << ")";
// }
