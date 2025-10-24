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
#include "codes_of_commands.h"
#include "spu_dump_funcs.h"
#include "spu_consts.h"
#include "spu_general_funcs.h"
#include "spu_read_funcs.h"
#include "spu_help_funcs.h"
#include "../size_of_file.h"

extern FILE* logfileProc;

int SpuReadCodeFromExecFile(Spu* spu, const char* execFileName)
{
    FILE* fp = fopen(execFileName, "rb");
    if (fp == NULL)
    {
        PRINT_LOG_FILE_SPU("ERROR: An error occurred while opening execFile \"%s\"\n", execFileName);
        return 1;
    }

    size_t sizeOfFile = SizeOfFile(fp);

    spu->lengthOfCode = sizeOfFile / sizeof(int) - HEADER_OFFSET;
    spu->code = (int*) calloc(spu->lengthOfCode, sizeof(int));
    if (spu->code == NULL)
    {
        PRINT_LOG_FILE_SPU("ERROR: An error occurred while creating buffer for reading execFile\n");
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
        PRINT_LOG_FILE_SPU("ERROR: An error occurred while opening outputFile \"%s\"\n", outputFileName);
        return 1;
    }
    spu->outputFile = fp;

    return 0;
}

int SpuCheckSignature(int* headerOfFile)
{
    if (headerOfFile[0] != VERSION)
    {
        PRINT_LOG_FILE_SPU("ERROR: Version of processor and executable file are not same!\n");
        return 1;
    }

    if (headerOfFile[1] != SIGNATURE_1 || headerOfFile[2] != SIGNATURE_2 || headerOfFile[3] != SIGNATURE_3)
    {
        PRINT_LOG_FILE_SPU("ERROR: The file signature is incorrect.\n");
        return 1;
    }

    return 0;
}
