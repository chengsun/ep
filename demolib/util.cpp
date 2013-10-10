#include "demolib.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#ifndef WINDOWS
#include <execinfo.h>
#endif

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

void dlib_stacktrace()
{
#ifndef WINDOWS
    void *array[10];
    size_t size = backtrace(array, 10);
    backtrace_symbols_fd(array, size, 2);
#endif
}
