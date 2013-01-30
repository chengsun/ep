#ifndef TYPES_H
#define TYPES_H

#include "util.h"
#include <GL/glew.h>
#include <cstdint>
#include <cmath>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
static constexpr U32 U32_MIN = static_cast<U32>(0);
static constexpr U32 U32_MAX = ~static_cast<U32>(0);
typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;

/******************************************************************************
 * Vector common ops
 *****************************************************************************/

#define VECTOR_DECL(VecType) \
    float &operator[](const size_t i); \
    constexpr float operator[](const size_t i); \
    constexpr VecType operator+(const VecType &b) const; \
    constexpr VecType operator-(const VecType &b) const; \
    constexpr VecType operator*(float f) const; \
    constexpr VecType operator/(float f) const; \
    VecType &operator+=(const VecType &b); \
    VecType &operator-=(const VecType &b); \
    VecType &operator*=(float f);

#define VECTOR_DEFS(VecType) \
    inline constexpr VecType sub(const VecType &a, const VecType &b) { \
        return add(a, neg(b)); \
    } \
    inline constexpr float hypot2(const VecType &a) { \
        return dot(a, a); \
    } \
    inline constexpr float hypot2(const VecType &a, const VecType &b) { \
        return hypot2(a - b); \
    } \
    inline constexpr float hypot(const VecType &a) { \
        return sqrtf(hypot2(a)); \
    } \
    inline constexpr float hypot(const VecType &a, const VecType &b) { \
        return sqrtf(hypot2(a, b)); \
    } \
    inline constexpr VecType normalize(const VecType &a) { \
        return scalediv(a, hypot(a)); \
    } \
    inline float &VecType::operator[](const size_t i) { \
        ASSERTX(i < VecType::size()); \
        return (reinterpret_cast<float *>(this))[i]; \
    } \
    inline constexpr float VecType::operator[](const size_t i) { \
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
    MatType::ColType &operator[](size_t i); \
    constexpr MatType::ColType const &operator[](size_t i);

#define MATRIX_DEFS(MatType) \
    inline MatType::ColType &MatType::operator[](size_t i) { \
        ASSERTX(i < MatType::nCols()); \
        return v[i]; \
    } \
    inline constexpr MatType::ColType const &MatType::operator[](size_t i) { \
        return v[i]; \
    } \

/******************************************************************************
 * Mat2
 *****************************************************************************/

struct Mat2
{
    typedef Vec2 ColType;
    ColType v[2];

    static constexpr unsigned nCols() { return 2; }

    MATRIX_DECL(Mat2)
};

inline constexpr Mat2 add(const Mat2 &a, const Mat2 &b) {
    return {{add(a.v[0], b.v[0]),
             add(a.v[1], b.v[1])}};
}
inline constexpr Mat2 neg(const Mat2 &a) {
    return {{neg(a.v[0]),
             neg(a.v[1])}};
}
inline constexpr Mat2 scale(const Mat2 &a, float f) {
    return {{scale(a.v[0], f),
             scale(a.v[1], f)}};
}
inline constexpr Mat2 scalediv(const Mat2 &a, float f) {
    return {{scalediv(a.v[0], f),
             scalediv(a.v[1], f)}};
}
inline constexpr Mat2 mul(const Mat2 &a, const Mat2 &b) {
    return {{{a.v[0][0]*b.v[0][0] + a.v[1][0]*b.v[0][1],
              a.v[0][1]*b.v[0][0] + a.v[1][1]*b.v[0][1]},
             {a.v[0][0]*b.v[1][0] + a.v[1][0]*b.v[1][1],
              a.v[0][1]*b.v[1][0] + a.v[1][1]*b.v[1][1]}}};
}

MATRIX_DEFS(Mat2)


/******************************************************************************
 * Mat3
 *****************************************************************************/

struct Mat3
{
    typedef Vec3 ColType;
    ColType v[3];
};


/******************************************************************************
 * Mat4
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
