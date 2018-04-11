#pragma once

#include <algorithm>

#include <cmath>

struct Vec3 {

    Vec3() {
        zero();
    }

    Vec3(float c) {
        set(c);
    }

    Vec3(float x, float y, float z) {
        set(x, y, z);
    }

    Vec3(const Vec3 &v) {
        set(v.x, v.y, v.z);
    }

    void zero() {
        set(0.f);
    }

    void set(float c) {
        set(c, c, c);
    }

    void set(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vec3 cross(const Vec3 &b) const {
         return Vec3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
    }

    void invert() {
        *this = inverted();
    }

    Vec3 inverted() const {
        return Vec3(1.f / x, 1.f / y, 1.f / z);
    }

    float dot(const Vec3 &b) const {
        return x * b.x + y * b.y + z * b.z;
    }

    float length() const {
        return std::sqrt(lengthSqr());
    }

    float lengthSqr() const {
        return x * x + y * y + z * z;
    }

    void normalize() {
        *this = normalized();
    }

    Vec3 normalized() const {
        float invLength = 1.f / length();
        return Vec3(invLength * x, invLength * y, invLength * z);
    }

    Vec3 abs() const {
        return Vec3(std::abs(x), std::abs(y), std::abs(z));
    }

    bool isNan() const {
        return std::isnan(x) || std::isnan(y) || std::isnan(z);
    }

    Vec3 minComponents(const Vec3 &v) const {
        return Vec3(std::min(x, v.x), std::min(y, v.y), std::min(z, v.z));
    }

    Vec3 maxComponents(const Vec3 &v) const {
        return Vec3(std::max(x, v.x), std::max(y, v.y), std::max(z, v.z));
    }

    float operator[](const int index) const {
        return a[index];
    }

    float &operator[](const int index) {
        return a[index];
    }

    Vec3 &operator+=(const Vec3 &b) {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }

    Vec3 &operator-=(const Vec3 &b) {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        return *this;
    }

    Vec3 &operator*=(const Vec3 &b) {
        x *= b.x;
        y *= b.y;
        z *= b.z;
        return *this;
    }

    Vec3 &operator/=(const Vec3 &b) {
        x /= b.x;
        y /= b.y;
        z /= b.z;
        return *this;
    }

    Vec3 &operator*=(float b) {
        x *= b;
        y *= b;
        z *= b;
        return *this;
    }

    Vec3 &operator/=(float b) {
        x /= b;
        y /= b;
        z /= b;
        return *this;
    }

     union {
         struct { float x, y, z; };
         float a[3];
     };
};

Vec3 operator-(const Vec3 &a);
Vec3 operator+(const Vec3 &a, const Vec3 &b);
Vec3 operator-(const Vec3 &a, const Vec3 &b);
Vec3 operator*(const Vec3 &a, const Vec3 &b);
Vec3 operator/(const Vec3 &a, const Vec3 &b);
Vec3 operator*(const Vec3 &a, float b);
Vec3 operator*(float a, const Vec3 &b);
Vec3 operator/(const Vec3 &a, float b);
Vec3 operator/(float a, const Vec3 &b);
bool operator>(const Vec3 &a, const Vec3 &b);
bool operator<(const Vec3 &a, const Vec3 &b);
bool operator>=(const Vec3 &a, const Vec3 &b);
bool operator<=(const Vec3 &a, const Vec3 &b);
bool operator==(const Vec3 &a, const Vec3 &b);
bool operator!=(const Vec3 &a, const Vec3 &b);
