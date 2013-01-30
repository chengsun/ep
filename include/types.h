#ifndef TYPES_H
#define TYPES_H

#include "util.h"
#include <GL/glew.h>
#include <cstdint>
#include <cmath>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
static const U32 U32_MIN = static_cast<U32>(0);
static const U32 U32_MAX = ~static_cast<U32>(0);
typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;

/******************************************************************************
 * Vector common ops
 *****************************************************************************/

#define VECTOR_DECL(VecType) \
    static constexpr VecType sub(const VecType &a, const VecType &b) { \
        return VecType::add(a, VecType::neg(b)); \
    } \
    static constexpr float hypot2(const VecType &a) { \
        return VecType::dot(a, a); \
    } \
    static constexpr float hypot2(const VecType &a, const VecType &b) { \
        return VecType::hypot2(a - b); \
    } \
    static constexpr float hypot(const VecType &a) { \
        return sqrtf(VecType::hypot2(a)); \
    } \
    static constexpr float hypot(const VecType &a, const VecType &b) { \
        return sqrtf(VecType::hypot2(a, b)); \
    } \
    static constexpr VecType normalize(const VecType &a) { \
        return VecType::scalediv(a, VecType::hypot(a)); \
    } \
    \
    float &operator[](const unsigned i) { \
        ASSERTX(i < VecType::length()); \
        return (reinterpret_cast<float *>(this))[i]; \
    } \
    float const &operator[](const unsigned i) const { \
        ASSERTX(i < VecType::length()); \
        return (reinterpret_cast<const float *>(this))[i]; \
    } \
    constexpr VecType operator+(const VecType &b) const { \
        return VecType::add(*this, b); \
    } \
    constexpr VecType operator-(const VecType &b) const { \
        return VecType::sub(*this, b); \
    } \
    constexpr VecType operator*(float f) const { \
        return VecType::scale(*this, f); \
    } \
    constexpr VecType operator/(float f) const { \
        return VecType::scalediv(*this, f); \
    } \
    VecType &operator+=(const VecType &b) { \
        *this = *this + b; \
        return *this; \
    } \
    VecType &operator-=(const VecType &b) { \
        *this = *this - b; \
        return *this; \
    } \
    VecType &operator*=(float f) { \
        *this = *this * f; \
        return *this; \
    }


/******************************************************************************
 * Vec2
 *****************************************************************************/

struct Vec2
{
    float x, y;

    static constexpr unsigned length() {
        return 2;
    }
    static constexpr Vec2 add(const Vec2 &a, const Vec2 &b) {
        return {a.x+b.x, a.y+b.y};
    }
    static constexpr Vec2 neg(const Vec2 &a) {
        return {-a.x, -a.y};
    }
    static constexpr Vec2 scale(const Vec2 &a, float f) {
        return {a.x*f, a.y*f};
    }
    static constexpr Vec2 scalediv(const Vec2 &a, float f) {
        return {a.x/f, a.y/f};
    }
    static constexpr float dot(const Vec2 &a, const Vec2 &b) {
        return a.x*b.x + a.y*b.y;
    }

    VECTOR_DECL(Vec2)
};

/******************************************************************************
 * Vec3
 *****************************************************************************/

struct Vec3
{
    float x, y, z;

    static constexpr unsigned length() {
        return 3;
    }
    static constexpr Vec3 add(const Vec3 &a, const Vec3 &b) {
        return {a.x+b.x, a.y+b.y, a.z+b.z};
    }
    static constexpr Vec3 neg(const Vec3 &a) {
        return {-a.x, -a.y, -a.z};
    }
    static constexpr Vec3 scale(const Vec3 &a, float f) {
        return {a.x*f, a.y*f, a.z*f};
    }
    static constexpr Vec3 scalediv(const Vec3 &a, float f) {
        return {a.x/f, a.y/f, a.z/f};
    }
    static constexpr float dot(const Vec3 &a, const Vec3 &b) {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }
    static constexpr Vec3 cross(const Vec3 &a, const Vec3 &b) {
        return {a.y*b.z - a.z*b.y,
                a.z*b.x - a.x*b.z,
                a.x*b.y - a.y*b.x};
    }

    VECTOR_DECL(Vec3)
};

/******************************************************************************
 * Vec4
 *****************************************************************************/

struct Vec4
{
    float x, y, z, w;

    static constexpr unsigned length() {
        return 4;
    }
    static constexpr Vec4 add(const Vec4 &a, const Vec4 &b) {
        return {a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w};
    }
    static constexpr Vec4 neg(const Vec4 &a) {
        return {-a.x, -a.y, -a.z, -a.w};
    }
    static constexpr Vec4 scale(const Vec4 &a, float f) {
        return {a.x*f, a.y*f, a.z*f, a.w*f};
    }
    static constexpr Vec4 scalediv(const Vec4 &a, float f) {
        return {a.x/f, a.y/f, a.z/f, a.w/f};
    }
    static constexpr float dot(const Vec4 &a, const Vec4 &b) {
        return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
    }

    VECTOR_DECL(Vec4)
};


/******************************************************************************
 * Mat2
 *****************************************************************************/

struct Mat2
{
    typedef Vec2 ColType;
    ColType v[2];
};


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
