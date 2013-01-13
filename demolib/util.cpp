#include "demolib.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>

void dlib_log(const char *file, unsigned line, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    time_t secs = time(0);
    tm *t = localtime(&secs);
    fprintf(stderr, "[%04d-%02d-%02d %02d:%02d:%02d] %s:%d: ",
            t->tm_year+1900, t->tm_mon+1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            file, line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);
}
