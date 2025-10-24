#ifndef SPU_GENERAL_FUNCS_HEADER
#define SPU_GENERAL_FUNCS_HEADER

#include "spu_structs.h"

int SpuExecProgram(Spu* spu);

int SpuCtor(Spu* spu);

int SpuVerify(Spu* spu);

int SpuDtor(Spu* spu);

#endif
