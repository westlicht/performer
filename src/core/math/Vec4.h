#pragma once

#include "Vec3.h"

#include <algorithm>

#include <cmath>

struct Vec4 {

    Vec4() {
        zero();
    }

    Vec4(float c) {
        set(c);
    }

    Vec4(float x, float y, float z, float w) {
        set(x, y, z, w);
    }

    Vec4(const Vec4 &v) {
        set(v.x, v.y, v.z, v.w);
    }

    Vec4(const Vec3 &v) {
        set(v.x, v.y, v.z, 1.f);
    }

    void zero() {
        set(0.f);
    }

    void set(float c) {
        set(c, c, c, c);
    }

    void set(float x, float y, float z, float w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    void invert() {
        *this = inverted();
    }

    Vec4 inverted() const {
        return Vec4(1.f / x, 1.f / y, 1.f / z, 1.f / w);
    }

    float dot(const Vec4 &b) const {
        return x * b.x + y * b.y + z * b.z + w * b.w;
    }

    float length() const {
        return std::sqrt(lengthSqr());
    }

    float lengthSqr() const {
        return x * x + y * y + z * z + w * w;
    }

    void normalize() {
        *this = normalized();
    }

    Vec4 normalized() const {
        float invLength = 1.f / length();
        return Vec4(invLength * x, invLength * y, invLength * z, invLength * w);
    }

    Vec4 abs() const {
        return Vec4(std::abs(x), std::abs(y), std::abs(z), std::abs(w));
    }

    bool isNan() const {
        return std::isnan(x) || std::isnan(y) || std::isnan(z) || std::isnan(w);
    }

    Vec4 minComponents(const Vec4 &v) const {
        return Vec4(std::min(x, v.x), std::min(y, v.y), std::min(z, v.z), std::min(w, v.w));
    }

    Vec4 maxComponents(const Vec4 &v) const {
        return Vec4(std::max(x, v.x), std::max(y, v.y), std::max(z, v.z), std::max(w, v.w));
    }

    float operator[](const int index) const {
        return a[index];
    }

    float &operator[](const int index) {
        return a[index];
    }

    Vec4 &operator+=(const Vec4 &b) {
        x += b.x;
        y += b.y;
        z += b.z;
        w += b.w;
        return *this;
    }

    Vec4 &operator-=(const Vec4 &b) {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        w -= b.w;
        return *this;
    }

    Vec4 &operator*=(const Vec4 &b) {
        x *= b.x;
        y *= b.y;
        z *= b.z;
        w *= b.w;
        return *this;
    }

    Vec4 &operator/=(const Vec4 &b) {
        x /= b.x;
        y /= b.y;
        z /= b.z;
        w /= b.w;
        return *this;
    }

    Vec4 &operator*=(float b) {
        x *= b;
        y *= b;
        z *= b;
        w *= b;
        return *this;
    }

    Vec4 &operator/=(float b) {
        x /= b;
        y /= b;
        z /= b;
        w /= b;
        return *this;
    }

     union {
         struct { float x, y, z, w; };
         float a[4];
     };
};

Vec4 operator-(const Vec4 &a);
Vec4 operator+(const Vec4 &a, const Vec4 &b);
Vec4 operator-(const Vec4 &a, const Vec4 &b);
Vec4 operator*(const Vec4 &a, const Vec4 &b);
Vec4 operator/(const Vec4 &a, const Vec4 &b);
Vec4 operator*(const Vec4 &a, float b);
Vec4 operator*(float a, const Vec4 &b);
Vec4 operator/(const Vec4 &a, float b);
Vec4 operator/(float a, const Vec4 &b);
bool operator>(const Vec4 &a, const Vec4 &b);
bool operator<(const Vec4 &a, const Vec4 &b);
bool operator>=(const Vec4 &a, const Vec4 &b);
bool operator<=(const Vec4 &a, const Vec4 &b);
bool operator==(const Vec4 &a, const Vec4 &b);
bool operator!=(const Vec4 &a, const Vec4 &b);
