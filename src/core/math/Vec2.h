#pragma once

#include <algorithm>

#include <cmath>

struct Vec2 {

    Vec2() {
        zero();
    }

    Vec2(float c) {
        set(c);
    }

    Vec2(float x, float y) {
        set(x, y);
    }

    Vec2(const Vec2 &v) {
        set(v.x, v.y);
    }

    void zero() {
        set(0.f);
    }

    void set(float c) {
        set(c, c);
    }

    void set(float x, float y) {
        this->x = x;
        this->y = y;
    }

    void invert() {
        *this = inverted();
    }

    Vec2 inverted() const {
        return Vec2(1.f / x, 1.f / y);
    }

    Vec2 perp() const {
        return Vec2(y, -x);
    }

    float dot(const Vec2 &b) const {
        return x * b.x + y * b.y;
    }

    float length() const {
        return std::sqrt(lengthSqr());
    }

    float lengthSqr() const {
        return x * x + y * y;
    }

    void normalize() {
        *this = normalized();
    }

    Vec2 normalized() const {
        float invLength = 1.f / length();
        return Vec2(x * invLength, y * invLength);
    }

    Vec2 abs() const {
        return Vec2(std::abs(x), std::abs(y));
    }

    bool isNan() const {
        return std::isnan(x) || std::isnan(y);
    }

    Vec2 minComponents(const Vec2 &v) const {
        return Vec2(std::min(x, v.x), std::min(y, v.y));
    }

    Vec2 maxComponents(const Vec2 &v) const {
        return Vec2(std::max(x, v.x), std::max(y, v.y));
    }

    float operator[](const int index) const {
        return a[index];
    }

    float &operator[](const int index) {
        return a[index];
    }

    Vec2 &operator+=(const Vec2 &b) {
        x += b.x;
        y += b.y;
        return *this;
    }

    Vec2 &operator-=(const Vec2 &b) {
        x -= b.x;
        y -= b.y;
        return *this;
    }

    Vec2 &operator*=(const Vec2 &b) {
        x *= b.x;
        y *= b.y;
        return *this;
    }

    Vec2 &operator/=(const Vec2 &b) {
        x /= b.x;
        y /= b.y;
        return *this;
    }

    Vec2 &operator*=(float b) {
        x *= b;
        y *= b;
        return *this;
    }

    Vec2 &operator/=(float b) {
        x /= b;
        y /= b;
        return *this;
    }

     union {
         struct { float x, y; };
         float a[2];
     };
};

Vec2 operator-(const Vec2 &a);
Vec2 operator+(const Vec2 &a, const Vec2 &b);
Vec2 operator-(const Vec2 &a, const Vec2 &b);
Vec2 operator*(const Vec2 &a, const Vec2 &b);
Vec2 operator/(const Vec2 &a, const Vec2 &b);
Vec2 operator*(const Vec2 &a, float b);
Vec2 operator*(float a, const Vec2 &b);
Vec2 operator/(const Vec2 &a, float b);
Vec2 operator/(float a, const Vec2 &b);
bool operator>(const Vec2 &a, const Vec2 &b);
bool operator<(const Vec2 &a, const Vec2 &b);
bool operator>=(const Vec2 &a, const Vec2 &b);
bool operator<=(const Vec2 &a, const Vec2 &b);
bool operator==(const Vec2 &a, const Vec2 &b);
bool operator!=(const Vec2 &a, const Vec2 &b);
