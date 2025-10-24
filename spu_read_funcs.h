#ifndef SPU_READ_FUNCS_HEADER
#define SPU_READ_FUNCS_HEADER

#include "spu_structs.h"

int SpuReadCodeFromExecFile(Spu* spu, const char* execFileName);

int SpuOpenOutputFile(Spu* spu, const char* outputFileName);

int SpuCheckSignature(int* headerOfFile);

#endif
