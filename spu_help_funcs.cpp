#include <stdio.h>

#include "spu_consts.h"
#include "spu_help_funcs.h"

extern FILE* logfileProc;

int SpuOpenLogFile()
{
    logfileProc = fopen(LOG_FILE_PROC_NAME, "w");
    if (logfileProc == NULL)
    {
        printf("ERROR: An error occurred while opening logfileProc.\n");
        return 1;
    }

    return 0;
}

void SpuCloseLogFile()
{
    fclose(logfileProc);
}
