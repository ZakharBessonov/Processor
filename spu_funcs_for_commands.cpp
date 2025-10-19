#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "spu_structs.h"
#include "stack.h"
#include "stackfunctions.h"
#include "spu_commands.h"
#include "spu_dump_funcs.h"
#include "spu_consts.h"
#include "spu_general_funcs.h"
#include "spu_read_funcs.h"
#include "spu_funcs_for_commands.h"

extern FILE* logfileProc;

int SpuPush(Spu* spu, int codeOfCommand)
{
    if (StackPush(&(spu->apparatStack), spu->code[spu->pc]))
    {
        fprintf(logfileProc, "ERROR: Error in apparatStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    (spu->pc)++;

    return 0;
}

int SpuPushReg(Spu* spu, int codeOfCommand)
{
    if (StackPush(&(spu->apparatStack), spu->registers[spu->code[spu->pc]]))
    {
        fprintf(logfileProc, "ERROR: Error in apparatStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    (spu->pc)++;

    return 0;
}

int SpuPop(Spu* spu, int codeOfCommand)
{
    int temp = 0;

    if (StackPop(&(spu->apparatStack), &temp))
    {
        fprintf(logfileProc, "ERROR: Error in apparatStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    return 0;
}

int SpuPopReg(Spu* spu, int codeOfCommand)
{
    int temp = 0;

    if (StackPop(&(spu->apparatStack), &temp))
    {
        fprintf(logfileProc, "ERROR: Error in apparatStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    spu->registers[spu->code[spu->pc]] = temp;
    (spu->pc)++;

    return 0;
}

int SpuSqr(Spu* spu, int codeOfCommand)
{
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
        fprintf(logfileProc, "ERROR: Error in apparatStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return IsBadResult;
    }

    return 0;
}

int SpuRet(Spu* spu, int codeOfCommand)
{
    int addressOfReturn = 0;
    if (StackPop(&(spu->returnRegisters), &addressOfReturn))
    {
        fprintf(logfileProc, "ERROR: Error in apparatStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }
    spu->pc = (size_t)addressOfReturn;

    return 0;
}

int SpuIn(Spu* spu, int codeOfCommand)
{
    int temp = 0;

    if (scanf("%d", &temp) <= 0)
    {
        fprintf(logfileProc, "ERROR: An error occurred while reading input "
                             "in command \"%s\".\n", cmds[codeOfCommand].name);
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
    if (spu->code[spu->pc] < 0 || (size_t)spu->code[spu->pc] >= spu->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address "
                             "in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    spu->pc = (size_t)spu->code[spu->pc];
    return 0;
}

int SpuCall(Spu* spu, int codeOfCommand)
{
    if (spu->code[spu->pc] < 0 || (size_t)spu->code[spu->pc] >= spu->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address "
                             "in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    if (StackPush(&(spu->returnRegisters),(int)(spu->pc) + 1))
    {
        fprintf(logfileProc, "ERROR: Error in apparatStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }
    spu->pc = (size_t)spu->code[spu->pc];

    return 0;
}

int SpuBasicArithmetic(Spu* spu, int codeOfCommand)
{
    int temp1 = 0, temp2 = 0;
    bool IsBadResult = StackPop(&(spu->apparatStack), &temp1) || StackPop(&(spu->apparatStack), &temp2);

    if (temp1 == 0 && (codeOfCommand == CMD_DIV || codeOfCommand == CMD_MOD))
    {
        fprintf(logfileProc, "ERROR: Division by zero in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    switch(codeOfCommand)
    {
        case CMD_ADD:
            IsBadResult = IsBadResult || StackPush(&(spu->apparatStack), temp1 + temp2);
            break;
        case CMD_SUB:
            IsBadResult = IsBadResult || StackPush(&(spu->apparatStack), temp2 - temp1);
            break;
        case CMD_MUL:
            IsBadResult = IsBadResult || StackPush(&(spu->apparatStack), temp2 * temp1);
            break;
        case CMD_DIV:
            IsBadResult = IsBadResult || StackPush(&(spu->apparatStack), temp2 / temp1);
            break;
        case CMD_MOD:
            IsBadResult = IsBadResult || StackPush(&(spu->apparatStack), temp2 % temp1);
            break;
        default:
            break;
    }

    return IsBadResult;
}

int SpuJumpFunc(Spu* spu, int codeOfCommand)
{
    if (spu->code[spu->pc] < 0 || (size_t)spu->code[spu->pc] >= spu->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address "
                             "in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int IsBadResult = StackPop(&(spu->apparatStack), &temp2) || StackPop(&(spu->apparatStack), &temp1);

    if (IsBadResult)
    {
        fprintf(logfileProc, "ERROR: Error in apparatStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return IsBadResult;
    }

    bool isNeedJump = 0;

    switch (codeOfCommand)
    {
        case CMD_JB:
            isNeedJump = temp1 < temp2;
            break;
        case CMD_JBE:
            isNeedJump = temp1 <= temp2;
            break;
        case CMD_JA:
            isNeedJump = temp1 > temp2;
            break;
        case CMD_JAE:
            isNeedJump = temp1 >= temp2;
            break;
        case CMD_JE:
            isNeedJump = temp1 == temp2;
            break;
        case CMD_JNE:
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
    int gotNumberOrChar = 0;

    if (StackPop(&(spu->apparatStack), &gotNumberOrChar))
    {
        fprintf(logfileProc, "ERROR: Error in apparatStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    switch(codeOfCommand)
    {
        case CMD_OUT:
            fprintf(spu->outputFile, "%d ", gotNumberOrChar);
            break;
        case CMD_OUTC:
            fprintf(spu->outputFile, "%c ", gotNumberOrChar);
            break;
        default:
            break;
    }

    return 0;
}

int SpuPushm(Spu* spu, int codeOfCommand)
{
    if (StackPush(&(spu->apparatStack), spu->ram[spu->registers[spu->code[spu->pc]]]))
    {
        fprintf(logfileProc, "ERROR: Error in apparatStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    (spu->pc)++;

    return 0;
}

int SpuPopm(Spu* spu, int codeOfCommand)
{
    int temp = 0;

    if (StackPop(&(spu->apparatStack), &temp))
    {
        fprintf(logfileProc, "ERROR: Error in apparatStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    spu->ram[spu->registers[spu->code[spu->pc]]] = temp;
    (spu->pc)++;

    return 0;
}

int SpuDraw(Spu* spu, int codeOfCommand)
{
    for (int i = 0; i < SIZE_OF_RAM; i++)
    {
        fprintf(spu->outputFile, "%c", spu->ram[i]);

        if ((i + 1) % LENGTH_OF_ONE_SIDE_OF_RAM == 0)
        {
            fprintf(spu->outputFile, "\n");
        }
    }

    return 0;
}

int SpuHlt(Spu* spu, int codeOfCommand)
{
    return 1;
}






