#pragma once

#include <array>

namespace sim {

template<typename T, unsigned Dim>
class Vector {
public:
    Vector() = default;

    template<typename... Ts>
    explicit Vector(const T &a, const T &b, const Ts &... ts) : _v({{a, b, ts...}}) {}

    template<typename OtherT>
    explicit Vector(const Vector<OtherT, Dim> &other) {
        for (unsigned i = 0; i < Dim; ++i) {
            _v[i] = T(other[i]);
        }
    }

    explicit Vector(T c) { _v.fill(c); }

    const T &operator[](unsigned i) const { return _v[i]; }
    T &operator[](unsigned i) { return _v[i]; }

    const T &x() const { return _v[0]; }
    const T &y() const { static_assert(Dim >= 2, "vector has no y coordinate"); return _v[1]; }
    const T &z() const { static_assert(Dim >= 3, "vector has no z coordinate"); return _v[2]; }
    const T &w() const { static_assert(Dim >= 4, "vector has no w coordinate"); return _v[3]; }

    T &x() { return _v[0]; }
    T &y() { static_assert(Dim >= 2, "vector has no y coordinate"); return _v[1]; }
    T &z() { static_assert(Dim >= 3, "vector has no z coordinate"); return _v[2]; }
    T &w() { static_assert(Dim >= 4, "vector has no w coordinate"); return _v[3]; }

    Vector operator-() const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = -_v[i];
        }
        return result;
    }

    Vector operator+(const Vector &other) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i] + other._v[i];
        }
        return result;
    }

    Vector operator-(const Vector &other) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i] - other._v[i];
        }
        return result;
    }

    Vector operator*(const Vector &other) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i]*other._v[i];
        }
        return result;
    }

    Vector operator/(const Vector &other) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i]/other._v[i];
        }
        return result;
    }

    Vector operator+(const T &a) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i] + a;
        }
        return result;
    }

    Vector operator-(const T &a) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i] - a;
        }
        return result;
    }

    Vector operator*(const T &a) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i]*a;
        }
        return result;
    }

    Vector operator/(const T &a) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i]/a;
        }
        return result;
    }

    T prod() const {
        T result = T(1);
        for (unsigned i = 0; i < Dim; ++i) {
            result *= _v[i];
        }
        return result;
    }

private:
    std::array<T, Dim> _v;
};

template<typename T, unsigned Dim>
Vector<T, Dim> operator+(const T &a, const Vector<T, Dim> &b) {
    Vector<T, Dim> result;
    for (unsigned i = 0; i < Dim; ++i) {
        result[i] = a + b[i];
    }
    return result;
}

template<typename T, unsigned Dim>
Vector<T, Dim> operator-(const T &a, const Vector<T, Dim> &b) {
    Vector<T, Dim> result;
    for (unsigned i = 0; i < Dim; ++i) {
        result[i] = a - b[i];
    }
    return result;
}

template<typename T, unsigned Dim>
Vector<T, Dim> operator*(const T &a, const Vector<T, Dim> &b) {
    Vector<T, Dim> result;
    for (unsigned i = 0; i < Dim; ++i) {
        result[i] = a*b[i];
    }
    return result;
}

template<typename T, unsigned Dim>
Vector<T, Dim> operator/(const T &a, const Vector<T, Dim> &b) {
    Vector<T, Dim> result;
    for (unsigned i = 0; i < Dim; ++i) {
        result[i] = a/b[i];
    }
    return result;
}

using Vector2i = Vector<int, 2>;
using Vector3i = Vector<int, 3>;
using Vector4i = Vector<int, 4>;

using Vector2f = Vector<float, 2>;
using Vector3f = Vector<float, 3>;
using Vector4f = Vector<float, 4>;

} // namespace sim
