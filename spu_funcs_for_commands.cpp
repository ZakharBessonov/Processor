#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "spu_structs.h"
#include "stack.h"
#include "stackfunctions.h"
#include "codes_of_commands.h"
#include "spu_dump_funcs.h"
#include "spu_consts.h"
#include "spu_general_funcs.h"
#include "spu_read_funcs.h"
#include "spu_funcs_for_commands.h"
#include "spu_help_funcs.h"

extern FILE* logfileProc;

int SpuPush(Spu* spu, int codeOfCommand)
{
    if (StackPush(&(spu->computationStack), spu->code[spu->pc]))
    {
        PRINT_LOG_FILE_SPU("ERROR: Error in computationStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    (spu->pc)++;

    return 0;
}

int SpuPushReg(Spu* spu, int codeOfCommand)
{
    if (StackPush(&(spu->computationStack), spu->registers[spu->code[spu->pc]]))
    {
        PRINT_LOG_FILE_SPU("ERROR: Error in computationStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    (spu->pc)++;

    return 0;
}

int SpuPop(Spu* spu, int codeOfCommand)
{
    int temp = 0;

    if (StackPop(&(spu->computationStack), &temp))
    {
        PRINT_LOG_FILE_SPU("ERROR: Error in computationStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    return 0;
}

int SpuPopReg(Spu* spu, int codeOfCommand)
{
    int temp = 0;

    if (StackPop(&(spu->computationStack), &temp))
    {
        PRINT_LOG_FILE_SPU("ERROR: Error in computationStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    spu->registers[spu->code[spu->pc]] = temp;
    (spu->pc)++;

    return 0;
}

int SpuSqr(Spu* spu, int codeOfCommand)
{
    int temp = 0;
    bool IsBadResult = StackPop(&(spu->computationStack), &temp);

    if (temp < 0)
    {
        PRINT_LOG_FILE_SPU("ERROR: Square root of a negative number in command \"sqr\".\n");
        return 1;
    }

    IsBadResult = IsBadResult || StackPush(&(spu->computationStack), (int)lround(sqrtl((long double)temp)));

    if (IsBadResult)
    {
        PRINT_LOG_FILE_SPU("ERROR: Error in computationStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return IsBadResult;
    }

    return 0;
}

int SpuRet(Spu* spu, int codeOfCommand)
{
    int addressOfReturn = 0;
    if (StackPop(&(spu->returnRegisters), &addressOfReturn))
    {
        PRINT_LOG_FILE_SPU("ERROR: Error in computationStack in command \"%s\".\n", cmds[codeOfCommand].name);
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
        PRINT_LOG_FILE_SPU("ERROR: An error occurred while reading input "
                           "in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    if (StackPush(&(spu->computationStack), temp))
    {
        return 1;
    }

    return 0;
}

int SpuJmp(Spu* spu, int codeOfCommand)
{
    if (spu->code[spu->pc] < 0 || (size_t)spu->code[spu->pc] >= spu->lengthOfCode)
    {
        PRINT_LOG_FILE_SPU("ERROR: Attempt to switch to a non-existent address "
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
        PRINT_LOG_FILE_SPU("ERROR: Attempt to switch to a non-existent address "
                           "in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    if (StackPush(&(spu->returnRegisters),(int)(spu->pc) + 1))
    {
        PRINT_LOG_FILE_SPU("ERROR: Error in computationStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }
    spu->pc = (size_t)spu->code[spu->pc];

    return 0;
}

int SpuBasicArithmetic(Spu* spu, int codeOfCommand)
{
    int temp1 = 0, temp2 = 0;
    bool IsBadResult = StackPop(&(spu->computationStack), &temp1) || StackPop(&(spu->computationStack), &temp2);

    if (temp1 == 0 && (codeOfCommand == CMD_DIV || codeOfCommand == CMD_MOD))
    {
        PRINT_LOG_FILE_SPU("ERROR: Division by zero in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    switch(codeOfCommand)
    {
        case CMD_ADD:
            IsBadResult = IsBadResult || StackPush(&(spu->computationStack), temp1 + temp2);
            break;
        case CMD_SUB:
            IsBadResult = IsBadResult || StackPush(&(spu->computationStack), temp2 - temp1);
            break;
        case CMD_MUL:
            IsBadResult = IsBadResult || StackPush(&(spu->computationStack), temp2 * temp1);
            break;
        case CMD_DIV:
            IsBadResult = IsBadResult || StackPush(&(spu->computationStack), temp2 / temp1);
            break;
        case CMD_MOD:
            IsBadResult = IsBadResult || StackPush(&(spu->computationStack), temp2 % temp1);
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
        PRINT_LOG_FILE_SPU("ERROR: Attempt to switch to a non-existent address "
                           "in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int IsBadResult = StackPop(&(spu->computationStack), &temp2) || StackPop(&(spu->computationStack), &temp1);

    if (IsBadResult)
    {
        PRINT_LOG_FILE_SPU("ERROR: Error in computationStack in command \"%s\".\n", cmds[codeOfCommand].name);
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

    if (StackPop(&(spu->computationStack), &gotNumberOrChar))
    {
        PRINT_LOG_FILE_SPU("ERROR: Error in computationStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    switch(codeOfCommand)
    {
        case CMD_OUT:
            fprintf(spu->outputFile, "%d ", gotNumberOrChar);
            break;
        case CMD_OUTC:
            fprintf(spu->outputFile, "%c", gotNumberOrChar);
            break;
        default:
            break;
    }

    return 0;
}

int SpuPushm(Spu* spu, int codeOfCommand)
{
    if (StackPush(&(spu->computationStack), spu->ram[spu->registers[spu->code[spu->pc]]]))
    {
        PRINT_LOG_FILE_SPU("ERROR: Error in computationStack in command \"%s\".\n", cmds[codeOfCommand].name);
        return 1;
    }

    (spu->pc)++;

    return 0;
}

int SpuPopm(Spu* spu, int codeOfCommand)
{
    int temp = 0;

    if (StackPop(&(spu->computationStack), &temp))
    {
        PRINT_LOG_FILE_SPU("ERROR: Error in computationStack in command \"%s\".\n", cmds[codeOfCommand].name);
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
        fprintf(spu->outputFile, "%c%c", spu->ram[i], spu->ram[i]);

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






