#ifndef PROCESSOR_FUNC_HEADER
#define PROCESSOR_FUNC_HEADER

#include "processor.h"

int Processor(const char* executeFile);

int DoProgram(SPU* stream);

int DoCommand(SPU* stream, int* numOfString);

int SPUCtor(SPU* stream, int* buffer, size_t lengthOfBuffer);

int SPUDump(SPU* stream);

int SPUVerify(SPU* stream);

int SPUCtor(SPU* stream);

int SPUDtor(SPU* stream);

int DoCommand(SPU* stream, int* numOfString);

int Push(SPU* stream, int* numOfString);

int PushReg(SPU* stream, int* numOfString);

int Pop(SPU* stream, int* numOfString);

int PopReg(SPU* stream, int* numOfString);

int In(SPU* stream, int* numOfString);

int Out(SPU* stream, int* numOfString);

int Add(SPU* stream, int* numOfString);

int Sub(SPU* stream, int* numOfString);

int Mul(SPU* stream, int* numOfString);

int Div(SPU* stream, int* numOfString);

int Mod(SPU* stream, int* numOfString);

int Sqr(SPU* stream, int* numOfString);

int Jmp(SPU* stream, int* numOfString);

int Jb(SPU* stream, int* numOfString);

int Jbe(SPU* stream, int* numOfString);

int Ja(SPU* stream, int* numOfString);

int Jae(SPU* stream, int* numOfString);

int Je(SPU* stream, int* numOfString);

int Jne(SPU* stream, int* numOfString);

#endif
