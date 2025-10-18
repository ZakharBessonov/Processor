#ifndef SPU_STRUCTS_HEADER
#define SPU_STRUCTS_HEADER

#include "stack.h"

const int CNT_OF_REGISTERS = 8;

struct Spu
{
    int*    code;
    size_t  lengthOfCode;
    size_t  pc;
    Stack   apparatStack;
    int     registers[CNT_OF_REGISTERS];
    Stack   returnRegisters;
    FILE*   outputFile;
};

enum SPUErrors
{
    SPU_OK = 0,
    NULL_SPU_POINTER = 1,
    NULL_CODE_POINTER = 2,
    BAD_PC = 4,
    BAD_APPARAT_STACK = 8,
    BAD_STACK_OF_RETURN_REGISTERS = 16
};

#endif
