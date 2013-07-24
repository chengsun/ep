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

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define ARRAYLEN(x) (sizeof(x)/sizeof((x)[0]))
#define PI 3.14159265359f

template <typename T>
inline T clamp(const T x, const T min, const T max)
{
    return std::max(min, std::min(x, max));
}

extern "C"
{
    void dlib_log(const char *file, unsigned line, const char *format, ...);
    void dlib_stacktrace();
}

#ifdef LOG
#undef LOG
#endif

#ifdef ASSERTX
#undef ASSERTX
#endif

#ifdef INFO
#undef INFO
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
            dlib_stacktrace(); \
            abort(); \
        } \
    } while (0)
#   define CHECK_GL_ERROR(where) \
    do { \
        GLenum glErr = glGetError(); \
        if (glErr) { \
            LOG("GL error: %s", gluErrorString(glErr)); \
            ASSERTX(!glErr, "GL error detected (%s)", where); \
        } \
    } while (0)
#else
#   define LOG(...)    do { (void) 0; } while (0)
#   define ASSERTX(x, ...) do { (void) sizeof(x); } while (0)
#   define CHECK_GL_ERROR(where) do { (void) sizeof(where); } while (0)
#endif


#include <cerrno>
#include <cstdlib>
inline void *dlib_malloc(size_t size, size_t alignment)
{
    void *ptr;
    int ret = posix_memalign(&ptr, alignment, size);
    ASSERTX(ret != EINVAL, "alignment invalid");
    ASSERTX(ret != ENOMEM, "insufficient memory");
    ASSERTX(ret == 0, "posix_memalign failed -- unknown reason");
    return ptr;
}



#include <mmintrin.h>
#include <xmmintrin.h>

#define ENABLE_FTZ() _mm_setcsr(_mm_getcsr() | 0x8040)


#define unlikely(expr) __builtin_expect(!!(expr), 0)
#define   likely(expr) __builtin_expect(!!(expr), 1)

template <class T>
T lerp(T a, T b, int num, int den)
{
    return a + (b-a)*num/den;
}

#endif
