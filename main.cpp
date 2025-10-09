#include <stdio.h>
#include <string.h>

#include "consts.h"
#include "processorFunc.h"

FILE* output = NULL;
FILE* logfileProc = NULL;
FILE* logfileStack = NULL;

#ifdef HASH_DJB2

uint64_t hashes[MAX_COUNT_OF_HASHES] = {};
size_t cntOfHashes = 0;

#endif

int main(int argc, const char *argv[])
{
    logfileProc = fopen(LOG_FILE_PROC_NAME, "w");
    logfileStack = fopen(LOG_FILE_STACK_NAME, "w");

    if (argc <= 2)
    {
        fprintf(logfileProc, "ERROR: Required files were not passed!\n");
        fclose(logfileProc);
        fclose(logfileStack);
        return 1;
    }

    if (strcmp(argv[2], "stdout") == 0)
    {
        output = stdout;
    }
    else
    {
        output = fopen(argv[2], "w");
    }

    Processor(argv[1]);

    fclose(logfileProc);
    fclose(logfileStack);
    fclose(output);

    return 0;
}
