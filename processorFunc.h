#ifndef PROCESSOR_FUNC_HEADER
#define PROCESSOR_FUNC_HEADER

#include "processor.h"

int Processor(const char* executeFile);

int DoProgram(SPU* process);

int DoCommand(SPU* process, int* numOfString);

int SPUCtor(SPU* process, int* buffer, size_t lengthOfBuffer);

int SPUVerify(SPU* process);

int SPUDtor(SPU* process);

int DoCommand(SPU* process, int* numOfString);

int Push(SPU* process, int* numOfString);

int PushReg(SPU* process, int* numOfString);

int Pop(SPU* process, int* numOfString);

int PopReg(SPU* process, int* numOfString);

int In(SPU* process, int* numOfString);

int Out(SPU* process, int* numOfString);

int Add(SPU* process, int* numOfString);

int Sub(SPU* process, int* numOfString);

int Mul(SPU* process, int* numOfString);

int Div(SPU* process, int* numOfString);

int Mod(SPU* process, int* numOfString);

int Sqr(SPU* process, int* numOfString);

int Jmp(SPU* process, int* numOfString);

int Jb(SPU* process, int* numOfString);

int Jbe(SPU* process, int* numOfString);

int Ja(SPU* process, int* numOfString);

int Jae(SPU* process, int* numOfString);

int Je(SPU* process, int* numOfString);

int Jne(SPU* process, int* numOfString);

#endif
