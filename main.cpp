#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spu_consts.h"
#include "spu_general_funcs.h"
#include "spu_read_funcs.h"
#include "spu_help_funcs.h"
#include "spu_structs.h"


FILE* logfileProc = NULL;

int main(int argc, const char *argv[])
{
    atexit(SpuCloseLogFile);

    if (SpuOpenLogFile())
    {
        return 0;
    }

    if (argc <= REQUIRED_NUMBER_OF_ARGUMENTS)
    {
        PRINT_LOG_FILE_SPU("ERROR: Too little arguments were passed.\n"
                            "       Input data format: execFileName.bin outputFileName.txt\n"
                            "                                               (or stdout)\n");
        return 0;
    }

    Spu spu = {};
    if (SpuCtor(&spu))
    {
        return 0;
    }

    if (SpuReadCodeFromExecFile(&spu, argv[1]) || SpuOpenOutputFile(&spu, argv[2]))
    {
        return 1;
    }

    if (SpuExecProgram(&spu))
    {
        return 0;
    }

    SpuDtor(&spu);

    return 0;
}
