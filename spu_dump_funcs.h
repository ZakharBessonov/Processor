#ifndef SPU_DUMP_FUNCS_HEADER
#define SPU_DUMP_FUNCS_HEADER

#include "spu_structs.h"

void StackDump(Stack* stk, const char* file, const char* func, int line);

int SpuDump(Spu* spu, int codeOfCommand);

void SpuPrintCode(Spu* spu);

void SpuPrintApparatStack(Spu* spu);

void SpuPrintRegisters(Spu* spu);

void SpuPrintReturnRegisters(Spu* spu);



#endif
