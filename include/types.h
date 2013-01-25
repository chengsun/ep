#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <GL/glew.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
static const U32 U32_MIN = static_cast<U32>(0);
static const U32 U32_MAX = ~static_cast<U32>(0);
typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;

struct Vec2
{
    float x;
    float y;
};

struct Vec3
{
    float x;
    float y;
    float z;


    static constexpr Vec3 add(const Vec3 &a, const Vec3 &b) {
        return {a.x+b.x, a.y+b.y, a.z+b.z};
    }
    static constexpr Vec3 sub(const Vec3 &a, const Vec3 &b) {
        return {a.x-b.x, a.y-b.y, a.z-b.z};
    }
    static constexpr Vec3 neg(const Vec3 &a) {
        return {-a.x, -a.y, -a.z};
    }
    static constexpr Vec3 scale(const Vec3 &a, float f) {
        return {a.x*f, a.y*f, a.z*f};
    }
    static constexpr Vec3 cross(const Vec3 &a, const Vec3 &b) {
        return {a.y*b.z - a.z*b.y,
                a.z*b.x - a.x*b.z,
                a.x*b.y - a.y*b.x};
    }
    static constexpr float dot(const Vec3 &a, const Vec3 &b) {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }
};

static constexpr Vec3 operator+(const Vec3 &a, const Vec3 &b) {
    return Vec3::add(a, b);
}
static constexpr Vec3 operator-(const Vec3 &a, const Vec3 &b) {
    return Vec3::sub(a, b);
}
static constexpr Vec3 operator*(const Vec3 &a, float f) {
    return Vec3::scale(a, f);
}
static constexpr Vec3 operator*(float f, const Vec3 &a) {
    return Vec3::scale(a, f);
}
inline static void operator+=(Vec3 &a, const Vec3 &b) {
    a = a + b;
}
inline static void operator-=(Vec3 &a, const Vec3 &b) {
    a = a - b;
}
inline static void operator*=(Vec3 &a, float f) {
    a = a * f;
}

struct Vec4
{
    float x, y, z, w;
    // TODO
};

struct Quat
{
    float x, y, z, w;
    // TODO
};

#endif
