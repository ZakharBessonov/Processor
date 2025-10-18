#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "spu_structs.h"
#include "stack.h"
#include "stackfunctions.h"
#include "commands.h"
#include "spu_dumpFuncs.h"
#include "spu_consts.h"
#include "spu_generalFuncs.h"
#include "spu_readFuncs.h"

extern FILE* logfileProc;

int SpuExecProgram(Spu* spu)
{
    int codeOfCommand = 0;
    while (spu->code[spu->pc] != CMD_HLT)
    {
        codeOfCommand = spu->code[spu->pc];
        if (codeOfCommand < 0 || codeOfCommand >= CNT_OF_COMMANDS)
        {
            fprintf(logfileProc, "ERROR: Unknown command on address [%d].\n", spu->pc);
            return 1;
        }

        if (commands[spu->code[spu->pc]].func(spu, command))
        {
            return 1;
        }
    }

    return 0;
}


int SpuCtor(Spu* spu, const char* execFileName, const char* outputFileName)
{
    if (SpuReadCodeFromExecFile(spu, execFileName) || SpuOpenOutputFile(spu, outputFileName))
    {
        return 1;
    }

    if (StackCtor(&(spu->apparatStack), 2) || StackCtor(&(spu->returnRegisters), 2))
    {
        return 1;
    }

    spu->pc = 0;

    return 0;
}

int SpuVerify(Spu* spu)
{
    int errors = SPU_OK;

    if (spu == NULL)
    {
        return NULL_SPU_POINTER;
    }

    if (spu->code == NULL)
    {
        errors |= NULL_CODE_POINTER;
    }

    if (spu->pc >= spu->lengthOfCode)
    {
        errors |= BAD_PC;
    }

    if (StackVerify(&(spu->apparatStack)))
    {
        errors |= BAD_APPARAT_STACK;
    }

    if (StackVerify(&(spu->returnRegisters)))
    {
        errors |= BAD_STACK_OF_RETURN_REGISTERS;
    }

    return errors;
}

int SpuDtor(Spu* spu)
{
    StackDtor(&(spu->apparatStack));
    StackDtor(&(spu->returnRegisters));

    free(spu->code);
    spu->code = NULL;
    spu->lengthOfCode = 0;
    spu->pc = 0;
    for (int i = 0; i < CNT_OF_REGISTERS; i++)
    {
        spu->registers[i] = 0;
    }
    fclose(spu->outputFile);

    return 0;
}
