#ifndef LINMAT_H
#define LINMAT_H

#include "util.h"
#include <GL/glew.h>
#include <cmath>

/******************************************************************************
 * Vector common ops
 *****************************************************************************/

#define VECTOR_DECL(VecType) \
    bool operator==(VecType const &v) const; \
    float &operator[](const size_t i); \
    const float &operator[](const size_t i) const; \
    constexpr VecType operator+(const VecType &b) const; \
    constexpr VecType operator-(const VecType &b) const; \
    constexpr VecType operator*(float f) const; \
    constexpr VecType operator/(float f) const; \
    VecType &operator+=(const VecType &b); \
    VecType &operator-=(const VecType &b); \
    VecType &operator*=(float f);

#define VECTOR_DEFS(VecType) \
    inline bool eq(const VecType &a, const VecType &b) { \
        for (unsigned x = 0; x < VecType::size(); ++x) { \
            if (a[x] != b[x]) return false; \
        } \
        return true; \
    } \
    inline bool VecType::operator==(VecType const &v) const { \
        return eq(*this, v); \
    } \
    inline constexpr VecType sub(const VecType &a, const VecType &b) { \
        return add(a, neg(b)); \
    } \
    inline constexpr float hypot2(const VecType &a) { \
        return dot(a, a); \
    } \
    inline constexpr float hypot2(const VecType &a, const VecType &b) { \
        return hypot2(a - b); \
    } \
    inline float hypot(const VecType &a) { \
        return sqrtf(hypot2(a)); \
    } \
    inline float hypot(const VecType &a, const VecType &b) { \
        return sqrtf(hypot2(a, b)); \
    } \
    inline VecType normalize(const VecType &a) { \
        return scalediv(a, hypot(a)); \
    } \
    inline float &VecType::operator[](const size_t i) { \
        ASSERTX(i < VecType::size()); \
        return (reinterpret_cast<float *>(this))[i]; \
    } \
    inline const float &VecType::operator[](const size_t i) const { \
        ASSERTX(i < VecType::size()); \
        return (reinterpret_cast<const float *>(this))[i]; \
    } \
    inline constexpr VecType VecType::operator+(const VecType &b) const { \
        return add(*this, b); \
    } \
    inline constexpr VecType VecType::operator-(const VecType &b) const { \
        return sub(*this, b); \
    } \
    inline constexpr VecType VecType::operator*(float f) const { \
        return scale(*this, f); \
    } \
    inline constexpr VecType VecType::operator/(float f) const { \
        return scalediv(*this, f); \
    } \
    inline VecType &VecType::operator+=(const VecType &b) { \
        *this = *this + b; \
        return *this; \
    } \
    inline VecType &VecType::operator-=(const VecType &b) { \
        *this = *this - b; \
        return *this; \
    } \
    inline VecType &VecType::operator*=(float f) { \
        *this = *this * f; \
        return *this; \
    }


/******************************************************************************
 * Vec2
 *****************************************************************************/

struct Vec2
{
    float x, y;

    static constexpr unsigned size() { return 2; }

    VECTOR_DECL(Vec2)
};

inline constexpr Vec2 add(const Vec2 &a, const Vec2 &b) {
    return {a.x+b.x, a.y+b.y};
}
inline constexpr Vec2 neg(const Vec2 &a) {
    return {-a.x, -a.y};
}
inline constexpr Vec2 scale(const Vec2 &a, float f) {
    return {a.x*f, a.y*f};
}
inline constexpr Vec2 scalediv(const Vec2 &a, float f) {
    return {a.x/f, a.y/f};
}
inline constexpr float dot(const Vec2 &a, const Vec2 &b) {
    return a.x*b.x + a.y*b.y;
}

VECTOR_DEFS(Vec2)


/******************************************************************************
 * Vec3
 *****************************************************************************/

struct Vec3
{
    float x, y, z;

    static constexpr unsigned size() { return 3; }

    VECTOR_DECL(Vec3)
};

inline constexpr Vec3 add(const Vec3 &a, const Vec3 &b) {
    return {a.x+b.x, a.y+b.y, a.z+b.z};
}
inline constexpr Vec3 neg(const Vec3 &a) {
    return {-a.x, -a.y, -a.z};
}
inline constexpr Vec3 scale(const Vec3 &a, float f) {
    return {a.x*f, a.y*f, a.z*f};
}
inline constexpr Vec3 scalediv(const Vec3 &a, float f) {
    return {a.x/f, a.y/f, a.z/f};
}
inline constexpr float dot(const Vec3 &a, const Vec3 &b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}
inline constexpr Vec3 cross(const Vec3 &a, const Vec3 &b) {
    return {a.y*b.z - a.z*b.y,
            a.z*b.x - a.x*b.z,
            a.x*b.y - a.y*b.x};
}

VECTOR_DEFS(Vec3)

/******************************************************************************
 * Vec4
 *****************************************************************************/

struct Vec4
{
    float x, y, z, w;

    static constexpr unsigned size() { return 4; }

    VECTOR_DECL(Vec4)
};

inline constexpr Vec4 add(const Vec4 &a, const Vec4 &b) {
    return {a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w};
}
inline constexpr Vec4 neg(const Vec4 &a) {
    return {-a.x, -a.y, -a.z, -a.w};
}
inline constexpr Vec4 scale(const Vec4 &a, float f) {
    return {a.x*f, a.y*f, a.z*f, a.w*f};
}
inline constexpr Vec4 scalediv(const Vec4 &a, float f) {
    return {a.x/f, a.y/f, a.z/f, a.w/f};
}
inline constexpr float dot(const Vec4 &a, const Vec4 &b) {
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

VECTOR_DEFS(Vec4)


/******************************************************************************
 * Matrix common ops
 *****************************************************************************/

#include <exception>

#define MATRIX_DECL(MatType) \
    static constexpr unsigned nRows() { \
        return MatType::ColType::size(); \
    } \
    static constexpr unsigned size() { \
        return nCols() * nRows(); \
    } \
    MatType transpose() const; \
    bool operator==(MatType const &m) const; \
    MatType::ColType &operator[](size_t i); \
    constexpr MatType::ColType const &operator[](size_t i); \
    constexpr MatType operator*(float f) const; \
    MatType operator*(MatType const &m) const;

#define MATRIX_DEFS(MatType) \
    inline bool eq(const MatType &a, const MatType &b) { \
        for (unsigned c = 0; c < MatType::nCols(); ++c) { \
            for (unsigned r = 0; r < MatType::nRows(); ++r) { \
                if (a[c][r] != b[c][r]) return false; \
            } \
        } \
        return true; \
    } \
    inline MatType MatType::transpose() const { \
        MatType m; \
        for (unsigned c = 0; c < MatType::nCols(); ++c) { \
            for (unsigned r = 0; r < MatType::nRows(); ++r) { \
                m[r][c] = (*this)[c][r]; \
            } \
        } \
        return m; \
    } \
    inline bool MatType::operator==(MatType const &m) const { \
        return eq(*this, m); \
    } \
    inline MatType::ColType &MatType::operator[](size_t i) { \
        ASSERTX(i < MatType::nCols()); \
        return v[i]; \
    } \
    inline constexpr MatType::ColType const &MatType::operator[](size_t i) { \
        return v[i]; \
    } \
    inline constexpr MatType MatType::operator*(float f) const { \
        return scale(*this, f); \
    } \
    inline MatType MatType::operator*(MatType const &m) const { \
        return mul(*this, m); \
    }

/******************************************************************************
 * Mat2
 *****************************************************************************/

struct Mat2
{
    typedef Vec2 ColType;
    ColType v[2];

    static constexpr unsigned nCols() { return 2; }
    static constexpr Mat2 identity() {
        return {{{1.f, 0.f},
                 {0.f, 1.f}}};
    }

    float det() const {
        return v[0][0]*v[1][1]
             - v[0][1]*v[1][0];
    }

    MATRIX_DECL(Mat2)
};

inline constexpr Mat2 add(const Mat2 &a, const Mat2 &b) {
    return {{add(a[0], b[0]),
             add(a[1], b[1])}};
}
inline constexpr Mat2 neg(const Mat2 &a) {
    return {{neg(a[0]),
             neg(a[1])}};
}
inline constexpr Mat2 scale(const Mat2 &a, float f) {
    return {{scale(a[0], f),
             scale(a[1], f)}};
}
inline constexpr Mat2 scalediv(const Mat2 &a, float f) {
    return {{scalediv(a[0], f),
             scalediv(a[1], f)}};
}
inline Mat2 mul(const Mat2 &a, const Mat2 &b) {
    return {{{a[0][0]*b[0][0] + a[1][0]*b[0][1],
              a[0][1]*b[0][0] + a[1][1]*b[0][1]},
             {a[0][0]*b[1][0] + a[1][0]*b[1][1],
              a[0][1]*b[1][0] + a[1][1]*b[1][1]}}};
}
inline Mat2::ColType mul(const Mat2 &m, const Mat2::ColType &v) {
    return {m[0][0]*v[0] + m[1][0]*v[1],
            m[0][1]*v[0] + m[1][1]*v[1]};
}

MATRIX_DEFS(Mat2)


/******************************************************************************
 * Mat3
 *****************************************************************************/

struct Mat3
{
    typedef Vec3 ColType;
    ColType v[3];

    static constexpr unsigned nCols() { return 3; }
    static constexpr Mat3 identity() {
        return {{{1.f, 0.f, 0.f},
                 {0.f, 1.f, 0.f},
                 {0.f, 0.f, 1.f}}};
    }

    float det() const {
        return v[0][0] * (v[1][1]*v[2][2] - v[2][1]*v[1][2])
             + v[1][0] * (v[2][1]*v[0][2] - v[0][1]*v[2][2])
             + v[2][0] * (v[0][1]*v[1][2] - v[1][1]*v[0][2]);
    }

    MATRIX_DECL(Mat3)
};

inline constexpr Mat3 add(const Mat3 &a, const Mat3 &b) {
    return {{add(a[0], b[0]),
             add(a[1], b[1]),
             add(a[2], b[2])}};
}
inline constexpr Mat3 neg(const Mat3 &a) {
    return {{neg(a[0]),
             neg(a[1]),
             neg(a[2])}};
}
inline constexpr Mat3 scale(const Mat3 &a, float f) {
    return {{scale(a[0], f),
             scale(a[1], f),
             scale(a[2], f)}};
}
inline constexpr Mat3 scalediv(const Mat3 &a, float f) {
    return {{scalediv(a[0], f),
             scalediv(a[1], f),
             scalediv(a[2], f)}};
}
inline Mat3 mul(const Mat3 &a, const Mat3 &b) {
    return {{{a[0][0]*b[0][0] + a[1][0]*b[0][1] + a[2][0]*b[0][2],
              a[0][1]*b[0][0] + a[1][1]*b[0][1] + a[2][1]*b[0][2],
              a[0][2]*b[0][0] + a[1][2]*b[0][1] + a[2][2]*b[0][2]},
             {a[0][0]*b[1][0] + a[1][0]*b[1][1] + a[2][0]*b[1][2],
              a[0][1]*b[1][0] + a[1][1]*b[1][1] + a[2][1]*b[1][2],
              a[0][2]*b[1][0] + a[1][2]*b[1][1] + a[2][2]*b[1][2]},
             {a[0][0]*b[2][0] + a[1][0]*b[2][1] + a[2][0]*b[2][2],
              a[0][1]*b[2][0] + a[1][1]*b[2][1] + a[2][1]*b[2][2],
              a[0][2]*b[2][0] + a[1][2]*b[2][1] + a[2][2]*b[2][2]}}};
}
inline Mat3::ColType mul(const Mat3 &m, const Mat3::ColType &v) {
    return {m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2],
            m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2],
            m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2]};
}

MATRIX_DEFS(Mat3)


/******************************************************************************
 * Mat4: TODO
 *****************************************************************************/

struct Mat4
{
    typedef Vec4 ColType;
    ColType v[4];
};


/******************************************************************************
 * Quat
 *****************************************************************************/

struct Quat
{
    float x, y, z, w;
    // TODO
};

#endif
