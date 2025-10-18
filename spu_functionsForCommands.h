#ifndef SPU_FUNCTIONS_FOR_COMMANDS_HEADER
#define SPU_FUNCTIONS_FOR_COMMANDS_HEADER

#include "processor.h"

int SpuPush(Spu* spu, int codeOfCommand);

int SpuPushReg(Spu* spu, int codeOfCommand);

int SpuPop(Spu* spu, int codeOfCommand);

int SpuPopReg(Spu* spu, int codeOfCommand);

int SpuIn(Spu* spu, int codeOfCommand);

int SpuSqr(Spu* spu, int codeOfCommand);

int SpuRet(Spu* spu, int codeOfCommand);

int SpuCall(Spu* spu, int codeOfCommand)

int SpuBasicArithmetic(Spu* spu, int codeOfCommand);

int SpuJumpFunc(Spu* spu, int codeOfCommand);

int SpuOutFunc(Spu* spu, int codeOfCommand);

int SpuHlt(Spu* spu, int codeOfCommand);

#endif
