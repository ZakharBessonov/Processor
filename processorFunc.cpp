#include <stdio.h>
#include <math.h>
#include <limits.h>

#include "processorFunc.h"
#include "stack.h"
#include "stackfunctions.h"
#include "commands.h"
#include "helpfunc.h"

extern FILE* output;
extern FILE* logfileProc;
extern FILE* logfileStack;

int Processor(const char* executeFile)
{
    FILE* fp = fopen(executeFile, "rb");
    if (fp == NULL)
    {
        fprintf(logfileProc, "ERROR: Failed to open executable file!\n");
        return 1;
    }

    size_t sizeOfFile = SizeOfFile(fp);

    size_t lengthOfBuffer = sizeOfFile / 4;
    int* buffer = (int*) calloc(lengthOfBuffer, sizeof(int));
    fread(buffer, sizeof(int), lengthOfBuffer, fp);

    if (buffer[0] != VERSION)
    {
        fprintf(logfileProc, "ERROR: Version of processor and executable file are not same!\n");
        return 1;
    }

    if (buffer[1] != 20 || buffer[2] != 2 || buffer[3] != 2007)
    {
        fprintf(logfileProc, "ERROR: The file signature is incorrect.\n");
        return 1;
    }

    SPU stream1 = {};
    if (SPUCtor(&stream1, buffer, lengthOfBuffer - 4))
    {
        fprintf(logfileProc, "ERROR: Failed to create stream.\n");
        return 1;
    }

    if (DoProgram(&stream1) != 1)
    {
        return 1;
    }

    free(buffer);
    StackDtor(&stream1.apparatStack);

    return 0;
}

int DoProgram(SPU* stream)
{
    if (SPUVerify(stream))
    {
        fprintf(logfileProc, "ERROR: Invalid stream was passed.\n");
    }

    int numOfString = 0;
    int result = 0;
    while ((result = DoCommand(stream, &numOfString)) == 0)
    {
        continue;
    }

    return result;
}


int SPUCtor(SPU* stream, int* buffer, size_t lengthOfBuffer)
{
    if (buffer == NULL)
    {
        fprintf(logfileProc, "ERROR: &buffer = NULL\n");
        return 1;
    }

    if (stream == NULL)
    {
        fprintf(logfileProc, "ERROR: &stream = NULL\n");
        return 1;
    }

    if (StackCtor(&(stream->apparatStack), 2))
    {
        return 1;
    }

    stream->code = buffer + 4;
    stream->lengthOfCode = lengthOfBuffer;
    stream->pc = 0;

    return 0;
}

int SPUVerify(SPU* stream)
{
    if (stream == NULL)
    {
        return NULL_SPU_POINTER;
    }

    if (stream->code == NULL)
    {
        return NULL_CODE_POINTER;
    }

    if (stream->pc >= stream->lengthOfCode)
    {
        return BAD_PC;
    }

    if (StackVerify(&(stream->apparatStack)))
    {
        return BAD_STACK;
    }

    if (stream->registers == NULL)
    {
        return BAD_REGISTERS;
    }

    return SPU_OK;
}

int DoCommand(SPU* stream, int* numOfString)
{
    //printf("%zu ", stream->pc);
    switch(stream->code[stream->pc])
    {
        case HLT:
            return 1;
        case PUSH:
            return Push(stream, numOfString);
        case PUSHREG:
            return PushReg(stream, numOfString);
        case POP:
            return Pop(stream, numOfString);
        case POPREG:
            return PopReg(stream, numOfString);
        case IN:
            return In(stream, numOfString);
        case OUT:
            return Out(stream, numOfString);
        case ADD:
            return Add(stream, numOfString);
        case SUB:
            return Sub(stream, numOfString);
        case MUL:
            return Mul(stream, numOfString);
        case DIV:
            return Div(stream, numOfString);
        case MOD:
            return Mod(stream, numOfString);
        case SQR:
            return Sqr(stream, numOfString);
        case JMP:
            return Jmp(stream, numOfString);
        case JB:
            return Jb(stream, numOfString);
        case JBE:
            return Jbe(stream, numOfString);
        case JA:
            return Ja(stream, numOfString);
        case JAE:
            return Jae(stream, numOfString);
        case JE:
            return Je(stream, numOfString);
        case JNE:
            return Jne(stream, numOfString);
        default:
            return 1;
    }
}

// Basic commands

int Push(SPU* stream, int* numOfString)
{
    (stream->pc)++;
    int result = StackPush(&(stream->apparatStack), stream->code[stream->pc]);
    (stream->pc)++;
    (*numOfString) += 2;

    return result;
}

int PushReg(SPU* stream, int* numOfString)
{
    (stream->pc)++;
    int result = StackPush(&(stream->apparatStack), stream->registers[stream->code[stream->pc]]);
    (stream->pc)++;
    (*numOfString) += 2;

    return result;
}

int Pop(SPU* stream, int* numOfString)
{
    (stream->pc)++;
    int temp = 0;
    int result = StackPop(&(stream->apparatStack), &temp);
    (*numOfString)++;

    return result;
}

int PopReg(SPU* stream, int* numOfString)
{
    (stream->pc)++;
    int temp = 0;
    int result = StackPop(&(stream->apparatStack), &temp);
    stream->registers[stream->code[stream->pc]] = temp;
    (stream->pc)++;
    (*numOfString) += 2;

    return result;
}

int Out(SPU* stream, int* numOfString)
{
    (stream->pc)++;
    int result = StackGetElem(&(stream->apparatStack));
    if (result == INT_MAX)
    {
        return 1;
    }
    fprintf(output, "%d ", result);
    (*numOfString)++;

    return 0;
}

int Add(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(stream->apparatStack), &temp1) || StackPop(&(stream->apparatStack), &temp2);
    result = result || StackPush(&(stream->apparatStack), temp1 + temp2);
    (*numOfString)++;

    return result;
}

int Sub(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(stream->apparatStack), &temp1) || StackPop(&(stream->apparatStack), &temp2);
    result = result || StackPush(&(stream->apparatStack), temp2 - temp1);
    (*numOfString)++;

    return result;
}

int Mul(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(stream->apparatStack), &temp1) || StackPop(&(stream->apparatStack), &temp2);
    result = result || StackPush(&(stream->apparatStack), temp1 * temp2);
    (*numOfString)++;

    return result;
}

int Div(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(stream->apparatStack), &temp1) || StackPop(&(stream->apparatStack), &temp2);
    result = result || StackPush(&(stream->apparatStack), temp2 / temp1);
    (*numOfString)++;

    return result;
}

int Mod(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(stream->apparatStack), &temp1) || StackPop(&(stream->apparatStack), &temp2);
    result = result || StackPush(&(stream->apparatStack), temp2 % temp1);
    (*numOfString)++;

    return result;
}

int Sqr(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    int temp = 0;
    int result = StackPop(&(stream->apparatStack), &temp);
    result = result || StackPush(&(stream->apparatStack), (int)lround(sqrtl((long double)temp)));
    (*numOfString)++;

    return result;
}

int Jmp(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    if (stream->code[stream->pc] < 0 || (size_t)stream->code[stream->pc] >= stream->lengthOfCode)
    {
        return 1;
    }
    (stream->pc)++;
    (*numOfString) += 2;

    stream->pc = (size_t)stream->code[stream->pc];
    return 0;
}

int Jb(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    if (stream->code[stream->pc] < 0 || (size_t)stream->code[stream->pc] >= stream->lengthOfCode)
    {
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(stream->apparatStack), &temp2) || StackPop(&(stream->apparatStack), &temp1);
    result = result || StackPush(&(stream->apparatStack), temp1);
    result = result || StackPush(&(stream->apparatStack), temp2);

    if (result)
    {
        return 1;
    }

    if (temp1 < temp2)
    {
        stream->pc = (size_t)stream->code[stream->pc];
    }
    else
    {
        (stream->pc)++;
    }
    (*numOfString) += 2;

    return 0;
}

int Jbe(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    if (stream->code[stream->pc] < 0 || (size_t)stream->code[stream->pc] >= stream->lengthOfCode)
    {
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(stream->apparatStack), &temp2) || StackPop(&(stream->apparatStack), &temp1);
    result = result || StackPush(&(stream->apparatStack), temp1);
    result = result || StackPush(&(stream->apparatStack), temp2);

    if (result)
    {
        return 1;
    }

    if (temp1 <= temp2)
    {
        stream->pc = (size_t)stream->code[stream->pc];
    }
    else
    {
        (stream->pc)++;
    }
    (*numOfString) += 2;

    return 0;
}

int Ja(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    if (stream->code[stream->pc] < 0 || (size_t)stream->code[stream->pc] >= stream->lengthOfCode)
    {
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(stream->apparatStack), &temp2) || StackPop(&(stream->apparatStack), &temp1);
    result = result || StackPush(&(stream->apparatStack), temp1);
    result = result || StackPush(&(stream->apparatStack), temp2);

    if (result)
    {
        return 1;
    }

    if (temp1 > temp2)
    {
        stream->pc = (size_t)stream->code[stream->pc];
    }
    else
    {
        (stream->pc)++;
    }
    (*numOfString) += 2;

    return 0;
}

int Jae(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    if (stream->code[stream->pc] < 0 || (size_t)stream->code[stream->pc] >= stream->lengthOfCode)
    {
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(stream->apparatStack), &temp2) || StackPop(&(stream->apparatStack), &temp1);
    result = result || StackPush(&(stream->apparatStack), temp1);
    result = result || StackPush(&(stream->apparatStack), temp2);

    if (result)
    {
        return 1;
    }

    if (temp1 >= temp2)
    {
        stream->pc = (size_t)stream->code[stream->pc];
    }
    else
    {
        (stream->pc)++;
    }
    (*numOfString) += 2;

    return 0;
}

int Je(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    if (stream->code[stream->pc] < 0 || (size_t)stream->code[stream->pc] >= stream->lengthOfCode)
    {
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(stream->apparatStack), &temp2) || StackPop(&(stream->apparatStack), &temp1);
    result = result || StackPush(&(stream->apparatStack), temp1);
    result = result || StackPush(&(stream->apparatStack), temp2);

    if (result)
    {
        return 1;
    }

    if (temp1 == temp2)
    {
        stream->pc = (size_t)stream->code[stream->pc];
    }
    else
    {
        (stream->pc)++;
    }
    (*numOfString) += 2;

    return 0;
}

int Jne(SPU* stream, int* numOfString)
{
    (stream->pc)++;

    if (stream->code[stream->pc] < 0 || (size_t)stream->code[stream->pc] >= stream->lengthOfCode)
    {
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(stream->apparatStack), &temp2) || StackPop(&(stream->apparatStack), &temp1);
    result = result || StackPush(&(stream->apparatStack), temp1);
    result = result || StackPush(&(stream->apparatStack), temp2);

    if (result)
    {
        return 1;
    }

    if (temp1 != temp2)
    {
        stream->pc = (size_t)stream->code[stream->pc];
    }
    else
    {
        (stream->pc)++;
    }
    (*numOfString) += 2;

    return 0;
}

int In(SPU* stream, int* numOfString)
{
    (stream->pc)++;
    int temp = 0;
    scanf("%d", &temp);
    int result = StackPush(&(stream->apparatStack), temp);
    (*numOfString)++;

    return result;
}





