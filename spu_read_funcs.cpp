#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "spu_structs.h"
#include "stack.h"
#include "stackfunctions.h"
#include "spu_commands.h"
#include "spu_dump_funcs.h"
#include "spu_consts.h"
#include "spu_general_funcs.h"
#include "spu_read_funcs.h"

extern FILE* logfileProc;

size_t SpuSizeOfFile(FILE* fp)
{
    int descriptor = fileno(fp);
    struct stat statistics = {};
    fstat(descriptor, &statistics);
    return (size_t)statistics.st_size;
}

int SpuReadCodeFromExecFile(Spu* spu, const char* execFileName)
{
    FILE* fp = fopen(execFileName, "rb");
    if (fp == NULL)
    {
        fprintf(logfileProc, "ERROR: An error occurred while opening execFile \"%s\"\n", execFileName);
        return 1;
    }

    size_t sizeOfFile = SpuSizeOfFile(fp);

    spu->lengthOfCode = sizeOfFile / sizeof(int) - HEADER_OFFSET;
    spu->code = (int*) calloc(spu->lengthOfCode, sizeof(int));
    if (spu->code == NULL)
    {
        fprintf(logfileProc, "ERROR: An error occurred while creating buffer for reading execFile\n");
        return 1;
    }

    int headerOfFile[HEADER_OFFSET] = {};
    fread(headerOfFile, sizeof(int), HEADER_OFFSET, fp);

    if (SpuCheckSignature(headerOfFile))
    {
        return 1;
    }
    fread(spu->code, sizeof(int), spu->lengthOfCode, fp);

    fclose(fp);

    return 0;
}

int SpuOpenOutputFile(Spu* spu, const char* outputFileName)
{
    if (strcmp(outputFileName, "stdout") == 0)
    {
        spu->outputFile = stdout;
        return 0;
    }

    FILE* fp = fopen(outputFileName, "w");
    if (fp == NULL)
    {
        fprintf(logfileProc, "ERROR: An error occurred while opening outputFile \"%s\"\n", outputFileName);
        return 1;
    }
    spu->outputFile = fp;

    return 0;
}

int SpuCheckSignature(int* headerOfFile)
{
    if (headerOfFile[0] != VERSION)
    {
        fprintf(logfileProc, "ERROR: Version of processor and executable file are not same!\n");
        return 1;
    }

    if (headerOfFile[1] != SIGNATURE_1 || headerOfFile[2] != SIGNATURE_2 || headerOfFile[3] != SIGNATURE_3)
    {
        fprintf(logfileProc, "ERROR: The file signature is incorrect.\n");
        return 1;
    }

    return 0;
}
