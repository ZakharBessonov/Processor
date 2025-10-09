#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "stack.h"
#include "consts.h"

extern FILE* logfileStack;

void PrintErrorInfo(const char* file, const char* func, int line, const char* message, ...)
{
    time_t var1 = time(NULL);
    fprintf(logfileStack, "%.24s %s %s:%d ", ctime(&var1), func, file, line);

    va_list arg_ptr;
    va_start(arg_ptr, message);
    vfprintf(logfileStack, message, arg_ptr);
    fflush(logfileStack);
    va_end(arg_ptr);
}

void FprintfStack_t(FILE* fp, Stack_t elem, const char* afterword)
{
    fprintf(fp, "%d%s", elem, afterword);
    fflush(fp);
}

size_t SizeOfFile(FILE* fp)
{
    int descriptor = fileno(fp);
    struct stat statistics = {};
    fstat(descriptor, &statistics);
    return (size_t)statistics.st_size;
}
