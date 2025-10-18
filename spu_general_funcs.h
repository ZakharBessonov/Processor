#ifndef SPU_GENERAL_FUNCS_HEADER
#define SPU_GENERAL_FUNCS_HEADER

int SpuExecProgram(Spu* spu);

int SpuCtor(Spu* spu, const char* execFileName, const char* outputFileName);

int SpuVerify(Spu* spu);

int SpuDtor(Spu* spu);

#endif