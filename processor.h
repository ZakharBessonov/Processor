#ifndef PROCESSOR_HEADER
#define PROCESSOR_HEADER

#include "stack.h"

const int CNT_OF_REGISTERS = 8;

struct SPU
{
    int* code;
    size_t lengthOfCode;
    size_t pc;
    Stack apparatStack;
    int registers[CNT_OF_REGISTERS];
};

enum SPUErrors
{
    SPU_OK = 0,
    NULL_SPU_POINTER = 1,
    NULL_CODE_POINTER = 2,
    BAD_PC = 4,
    BAD_STACK = 8,
    BAD_REGISTERS = 15
};

#endif
