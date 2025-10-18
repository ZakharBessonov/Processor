#include <stdio.h>
#include <string.h>

#include "spu_consts.h"
#include "spu_generalFuncs.h"
#include "spu_readFuncs.h"
#include "spu_structs.h"


FILE* logfileProc = NULL;

int main(int argc, const char *argv[])
{
    logfileProc = fopen(LOG_FILE_PROC_NAME, "w");

    if (argc <= REQUIRED_NUMBER_OF_ARGUMENTS)
    {
        fprintf(logfileAsm, "ERROR: Too little arguments were passed.\n"
                            "Input data format: execFileName.bin outputFileName.txt\n"
                            "                                      (or stdout)\n");
        fclose(logfileProc);
        return 1;
    }

    Spu spu = {};
    if (SpuCtor(spu, argv[1], argv[2]))
    {
        return 1;
    }

    if (SpuExecProgram(spu))
    {
        return 1;
    }

    SpuDtor(spu);
    fclose(logfileProc);

    return 0;
}
