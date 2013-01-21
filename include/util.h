#ifndef DEMOLIB_UTIL_H
#define DEMOLIB_UTIL_H

#include "types.h"

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

#ifdef DEBUG
#   include <cstdlib>
#   define LOG(...) dlib_log(__FILE__, __LINE__, __VA_ARGS__)
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


struct LCGRand
{
    U32 state;
    U32 get() {
        state = state * 1664525 + 1013904223;
        return state;
    }
    float getf() {
        return static_cast<float>(get())/U32_MAX;
    }
};

#endif
