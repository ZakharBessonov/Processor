#ifndef HELPFUNC_HEADER
#define HELPFUNC_HEADER

#include "processor.h"

void PrintErrorInfo(const char* file, const char* func, int line, const char* message, ...);

void FprintfStack_t(FILE* fp, Stack_t elem, const char* afterword);

size_t SizeOfFile(FILE* fp);

int IsThereCommandIAmongListed(int command, int numOfCommand, ...);

void PrintCode(SPU* process);

void PrintStack(SPU* process);

void PrintRegisters(SPU* process);

#endif
