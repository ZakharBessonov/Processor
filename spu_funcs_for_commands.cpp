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
#include "spu_functionsForCommands.h"

extern FILE* logfileProc;

int SpuPush(Spu* spu, int codeOfCommand)
{
    if (SpuVerify(spu))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"%s\".\n", commands[codeOfCommand].name);
        SpuDump(spu);
        return 1;
    }

    (spu->pc)++;

    if (StackPush(&(spu->apparatStack), spu->code[spu->pc]))
    {
        return 1;
    }

    (spu->pc)++;

    return 0;
}

int SpuPushReg(Spu* spu, int codeOfCommand)
{
    if (SpuVerify(spu))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"%s\".\n", commands[codeOfCommand].name);
        SpuDump(spu);
        return 1;
    }

    (spu->pc)++;

    if (StackPush(&(spu->apparatStack), spu->registers[spu->code[spu->pc]]))
    {
        return 1;
    }

    (spu->pc)++;

    return 0;
}

int SpuPop(Spu* spu, int codeOfCommand)
{
    if (SpuVerify(spu))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"%s\".\n", commands[codeOfCommand].name);
        SpuDump(spu);
        return 1;
    }

    (spu->pc)++;
    int temp = 0;

    if (StackPop(&(spu->apparatStack), &temp))
    {
        return 1;
    }

    return 0;
}

int SpuPopReg(Spu* spu, int codeOfCommand)
{
    if (SpuVerify(spu))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"%s\".\n", commands[codeOfCommand].name);
        SpuDump(spu);
        return 1;
    }

    (spu->pc)++;
    int temp = 0;

    if (StackPop(&(spu->apparatStack), &temp))
    {
        return 1;
    }

    spu->registers[spu->code[spu->pc]] = temp;
    (spu->pc)++;

    return 0;
}

int SpuSqr(Spu* spu, int codeOfCommand)
{
    if (SpuVerify(spu))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"%s\".\n", commands[codeOfCommand].name);
        SpuDump(spu);
        return 1;
    }

    (spu->pc)++;

    int temp = 0;
    bool IsBadResult = StackPop(&(spu->apparatStack), &temp);

    if (temp < 0)
    {
        fprintf(logfileProc, "ERROR: Square root of a negative number in command \"sqr\".\n");
        return 1;
    }

    IsBadResult = IsBadResult || StackPush(&(spu->apparatStack), (int)lround(sqrtl((long double)temp)));

    if (IsBadResult)
    {
        return IsBadResult;
    }

    return 0;
}

int SpuRet(Spu* spu, int codeOfCommand)
{
    if (SpuVerify(spu))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"%s\".\n", commands[codeOfCommand].name);
        SpuDump(spu);
        return 1;
    }

    int addressOfReturn = 0;
    if (StackPop(&(spu->returnRegisters), &addressOfReturn))
    {
        fprintf(logfileProc, "ERROR: Stack of return registers is empty (from command\"ret\").\n");
        return 1;
    }
    spu->pc = (size_t)addressOfReturn;

    return 0;
}

int SpuIn(Spu* spu, int codeOfCommand)
{
    if (SpuVerify(spu))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"%s\".\n", commands[codeOfCommand].name);
        SpuDump(spu);
        return 1;
    }

    (spu->pc)++;
    int temp = 0;

    if (scanf("%d", &temp) <= 0)
    {
        fprintf(logfileProc, "ERROR: An error occurred while reading input "
                             "in command \"%s\".\n", commands[codeOfCommand].name);
        return 1;
    }

    if (StackPush(&(spu->apparatStack), temp))
    {
        return 1;
    }

    return 0;
}

int SpuJmp(Spu* spu, int codeOfCommand)
{
    if (SpuVerify(spu))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"%s\".\n", commands[codeOfCommand].name);
        SpuDump(spu);
        return 1;
    }

    (spu->pc)++;

    if (spu->code[spu->pc] < 0 || (size_t)spu->code[spu->pc] >= spu->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address "
                             "in command \"%s\".\n", commands[codeOfCommand].name);
        return 1;
    }

    spu->pc = (size_t)spu->code[spu->pc];
    return 0;
}

int SpuCall(Spu* spu, int codeOfCommand)
{
    if (SpuVerify(spu))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"%s\".\n", commands[codeOfCommand].name);
        SpuDump(spu);
        return 1;
    }

    (spu->pc)++;

    if (spu->code[spu->pc] < 0 || (size_t)spu->code[spu->pc] >= spu->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address "
                             "in command \"%s\".\n", commands[codeOfCommand].name);
        return 1;
    }

    if (StackPush(&(spu->returnRegisters),(int)(spu->pc) + 1))
    {
        return 1;
    }
    spu->pc = (size_t)spu->code[spu->pc];

    return 0;
}

int SpuBasicArithmetic(Spu* spu, int codeOfCommand)
{
    if (SpuVerify(spu))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"%s\".\n", commands[codeOfCommand].name);
        SpuDump(spu);
        return 1;
    }

    (spu->pc)++;

    int temp1 = 0, temp2 = 0;
    bool IsBadResult = StackPop(&(spu->apparatStack), &temp1) || StackPop(&(spu->apparatStack), &temp2);

    switch(codeOfCommand)
    {
        case COMMAND_ADD:
            IsBadResult = IsBadResult || StackPush(&(spu->apparatStack), temp1 + temp2);
            break;
        case COMMAND_SUB:
            IsBadResult = IsBadResult || StackPush(&(spu->apparatStack), temp2 - temp1);
            break;
        case COMMAND_MUL:
            IsBadResult = IsBadResult || StackPush(&(spu->apparatStack), temp2 * temp1);
            break;
        case COMMAND_DIV:
            if (temp1 == 0)
            {
                fprintf(logfileProc, "ERROR: Division by zero in command \"div\".\n");
                return 1;
            }
            IsBadResult = IsBadResult || StackPush(&(spu->apparatStack), temp2 / temp1);
            break;
        case COMMAND_MOD:
            if (temp1 == 0)
            {
                fprintf(logfileProc, "ERROR: Division by zero in command \"mod\".\n");
                return 1;
            }
            IsBadResult = IsBadResult || StackPush(&(spu->apparatStack), temp2 % temp1);
            break;
        default:
            break;
    }

    if (IsBadResult)
    {
        return IsBadResult;
    }

    return 0;
}

int SpuJumpFunc(Spu* spu, int codeOfCommand)
{
    if (SpuVerify(spu))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"%s\".\n", commands[codeOfCommand].name);
        SpuDump(spu);
        return 1;
    }

    (spu->pc)++;

    if (spu->code[spu->pc] < 0 || (size_t)spu->code[spu->pc] >= spu->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address "
                             "in command \"%s\".\n", commands[codeOfCommand].name);
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int IsBadResult = StackPop(&(spu->apparatStack), &temp2) || StackPop(&(spu->apparatStack), &temp1);

    if (IsBadResult)
    {
        return IsBadResult;
    }

    bool isNeedJump = 0;

    switch (codeOfCommand)
    {
        case COMMAND_JB:
            isNeedJump = temp1 < temp2;
            break;
        case COMMAND_JBE:
            isNeedJump = temp1 <= temp2;
            break;
        case COMMAND_JA:
            isNeedJump = temp1 > temp2;
            break;
        case COMMAND_JAE:
            isNeedJump = temp1 >= temp2;
            break;
        case COMMAND_JE:
            isNeedJump = temp1 == temp2;
            break;
        case COMMAND_JNE:
            isNeedJump = temp1 != temp2;
            break;
        default:
            break;
    }

    if (isNeedJump)
    {
        spu->pc = (size_t)spu->code[spu->pc];
        return 0;
    }

    (spu->pc)++;

    return 0;
}

int SpuOutFunc(Spu* spu, int codeOfCommand)
{
    if (SpuVerify(spu))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"%s\".\n", commands[codeOfCommand].name);
        SpuDump(spu);
        return 1;
    }

    (spu->pc)++;
    int gotNumberOrChar = 0;

    if (StackPop(&(spu->apparatStack), &gotNumberOrChar))
    {
        return 1;
    }

    switch(codeOfCommand)
    {
        case COMMAND_OUT:
            fprintf(output, "%d ", gotNumberOrChar);
            break;
        case COMMAND_OUTC:
            fprintf(output, "%c ", gotNumberOrChar);
            break;
        default:
            break;
    }

    return 0;
}

int SpuHlt(Spu* spu, int codeOfCommand)
{
    return 1;
}






