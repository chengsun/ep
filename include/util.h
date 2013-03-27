#ifndef DEMOLIB_UTIL_H
#define DEMOLIB_UTIL_H

#include <memory>
#include <cinttypes>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
static constexpr U32 U32_MIN = static_cast<U32>(0);
static constexpr U32 U32_MAX = ~static_cast<U32>(0);
typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;


#define ARRAYLEN(x) (sizeof(x)/sizeof((x)[0]))
#define PI 3.14159265359f

extern "C"
{
    void dlib_log(const char *file, unsigned line, const char *format, ...);
}

#ifdef LOG
#undef LOG
#endif

#ifdef ASSERTX
#undef ASSERTX
#endif

#define INFO(...) dlib_log(__FILE__, __LINE__, __VA_ARGS__)

#ifdef DEBUG
#   include <cstdlib>
#   define LOG(...) INFO(__VA_ARGS__)
#   define ASSERTX(x, ...) \
    do { \
        if (!(x)) { \
            LOG("* ASSERTION '%s' FAILED! *", #x); \
            LOG("" __VA_ARGS__); \
            abort(); \
        } \
    } while (0)
#else
#   define LOG(...)    ((void)0)
#   define ASSERTX(x, ...) ((void)0)
#endif

#endif
