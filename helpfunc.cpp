#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "stack.h"
#include "consts.h"
#include "commands.h"
#include "processor.h"

extern FILE* logfileStack;
extern FILE* logfileProc;

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

int IsThereCommandIAmongListed(int command, int numOfCommand, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, numOfCommand);
    for (int i = 0; i < numOfCommand; i++)
    {
        if (command == va_arg(arg_ptr, int))
        {
            va_end(arg_ptr);
            return 1;
        }
    }

    va_end(arg_ptr);
    return 0;
}

void PrintCode(SPU* process)
{
    fprintf(logfileProc, "Code: ");

    if (process->pc <= 3)
    {
        for (int i = 0; i <= 3; i++)
        {
            fprintf(logfileProc, "%.4X ", (unsigned int)process->code[i]);
        }
        fprintf(logfileProc, "\n      %*c^^^^\n", 5 * (int)process->pc, ' ');
    }
    else
    {
        for (size_t i = process->pc; i <= 3 + process->pc; i++)
        {
            fprintf(logfileProc, "%.4X ", (unsigned int)process->code[i]);
        }
        fprintf(logfileProc, "\n                     ^^^^\n");
    }
}

void PrintStack(SPU* process)
{
    fprintf(logfileProc, "Stack: ");

    if (process->apparatStack.size == 0)
    {
        fprintf(logfileProc, "empty\n");
    }
    else
    {
        for (int i = 0; i < process->apparatStack.size; i++)
        {
            fprintf(logfileProc, "%d ", process->apparatStack.data[i]);
        }
        fprintf(logfileProc, "\n");
    }
}

void PrintRegisters(SPU* process)
{
    fprintf(logfileProc, "Registers: ");
    for (int i = 0; i < CNT_OF_REGISTERS; i++)
    {
        fprintf(logfileProc, "%cX = %d; ", i + 65, process->registers[i]);
    }
    fprintf(logfileProc, "\n");
}
