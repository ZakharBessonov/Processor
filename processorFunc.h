#ifndef PROCESSOR_FUNC_HEADER
#define PROCESSOR_FUNC_HEADER

#include "processor.h"

int Processor(const char* executeFile);

int DoProgram(SPU* process);

int DoCommand(SPU* process);

int SPUCtor(SPU* process, int* buffer, size_t lengthOfBuffer);

int SPUVerify(SPU* process);

int SPUDtor(SPU* process);

int DoCommand(SPU* process);

int Push(SPU* process);

int PushReg(SPU* process);

int Pop(SPU* process);

int PopReg(SPU* process);

int In(SPU* process);

int Out(SPU* process);

int Add(SPU* process);

int Sub(SPU* process);

int Mul(SPU* process);

int Div(SPU* process);

int Mod(SPU* process);

int Sqr(SPU* process);

int Jmp(SPU* process);

int Jb(SPU* process);

int Jbe(SPU* process);

int Ja(SPU* process);

int Jae(SPU* process);

int Je(SPU* process);

int Jne(SPU* process);

#endif
