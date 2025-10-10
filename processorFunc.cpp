#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "processorFunc.h"
#include "processor.h"
#include "stack.h"
#include "stackfunctions.h"
#include "commands.h"
#include "helpfunc.h"
#include "dumpfuncs.h"

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

    SPU process1 = {};
    if (SPUCtor(&process1, buffer, lengthOfBuffer - 4))        // -4 because first four numbers are the title

    {
        fprintf(logfileProc, "ERROR: Failed to create process.\n");
        return 1;
    }

    if (DoProgram(&process1) != 1)
    {
        return 1;
    }

    free(buffer);
    StackDtor(&process1.apparatStack);

    return 0;
}

int DoProgram(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid process was passed.\n");
    }

    int numOfString = 1;
    int result = 0;
    while ((result = DoCommand(process, &numOfString)) == 0)
    {
        continue;
    }

    return result;
}


int SPUCtor(SPU* process, int* buffer, size_t lengthOfBuffer)
{
    if (buffer == NULL)
    {
        fprintf(logfileProc, "ERROR: &buffer = NULL\n");
        return 1;
    }

    if (process == NULL)
    {
        fprintf(logfileProc, "ERROR: &process = NULL\n");
        return 1;
    }

    if (StackCtor(&(process->apparatStack), 2))
    {
        return 1;
    }

    process->code = buffer + 4;
    process->lengthOfCode = lengthOfBuffer;
    process->pc = 0;

    process->whereStartEveryString = (size_t*) calloc(lengthOfBuffer + 1, sizeof(size_t));
    size_t j = 1;
    for (size_t i = 0; i < lengthOfBuffer; i++)
    {
        if (IsThereCommandIAmongListed(process->code[i], 11, HLT, POP, OUT, ADD, SUB, MUL, DIV, MOD, SQR, IN, DMP))
        {
            process->whereStartEveryString[j] = i;
            //printf("%zu ", i);   // For debug
            j++;
        }
        else
        {
            process->whereStartEveryString[j] = i;
            //printf("%zu ", i);   // For debug
            i++;
            j++;
        }
    }
    process->cntOfLines = j - 1;

    return 0;
}

int SPUVerify(SPU* process)
{
    int errors = SPU_OK;

    if (process == NULL)
    {
        return NULL_SPU_POINTER;
    }

    if (process->code == NULL)
    {
        errors |= NULL_CODE_POINTER;
    }

    if (process->whereStartEveryString == NULL)
    {
        errors |= NULL_WHERE_START_EVERY_STRING_POINTER;
    }

    if (process->pc >= process->lengthOfCode)
    {
        errors |= BAD_PC;
    }

    if (process->cntOfLines > process->lengthOfCode)
    {
        errors |= BAD_CNT_OF_LINES;
    }

    if (StackVerify(&(process->apparatStack)))
    {
        errors |= BAD_STACK;
    }

    if (process->registers == NULL)
    {
        errors |= BAD_REGISTERS;
    }

    return errors;
}

int SPUDtor(SPU* process)
{
    int resultOfCheek = SPUVerify(process);
    if (resultOfCheek)
    {
        fprintf(logfileProc, "ERROR: Damaged process was passed!\n");
        return 1;
    }

    StackDtor(&(process->apparatStack));
    process->code = NULL;
    process->lengthOfCode = 0;
    process->pc = 0;
    free(process->whereStartEveryString);
    process->whereStartEveryString = NULL;
    memset(process->registers, 0, sizeof(int) * CNT_OF_REGISTERS);

    return 0;
}

int DoCommand(SPU* process, int* numOfString)
{
    //printf("%zu ", process->pc);
    switch(process->code[process->pc])
    {
        case HLT:
            return 1;
        case PUSH:
            return Push(process, numOfString);
        case PUSHREG:
            return PushReg(process, numOfString);
        case POP:
            return Pop(process, numOfString);
        case POPREG:
            return PopReg(process, numOfString);
        case IN:
            return In(process, numOfString);
        case DMP:
            SPUDump(process, numOfString);
            (process->pc)++;
            (*numOfString)++;
            return 0;
        case OUT:
            return Out(process, numOfString);
        case ADD:
            return Add(process, numOfString);
        case SUB:
            return Sub(process, numOfString);
        case MUL:
            return Mul(process, numOfString);
        case DIV:
            return Div(process, numOfString);
        case MOD:
            return Mod(process, numOfString);
        case SQR:
            return Sqr(process, numOfString);
        case JMP:
            return Jmp(process, numOfString);
        case JB:
            return Jb(process, numOfString);
        case JBE:
            return Jbe(process, numOfString);
        case JA:
            return Ja(process, numOfString);
        case JAE:
            return Jae(process, numOfString);
        case JE:
            return Je(process, numOfString);
        case JNE:
            return Jne(process, numOfString);
        default:
            return 1;
    }
}

// Basic commands

int Push(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;
    int result = StackPush(&(process->apparatStack), process->code[process->pc]);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    (process->pc)++;
    (*numOfString)++;

    return 0;
}

int PushReg(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;
    int result = StackPush(&(process->apparatStack), process->registers[process->code[process->pc]]);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    (process->pc)++;
    (*numOfString)++;

    return 0;
}

int Pop(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;
    int temp = 0;
    int result = StackPop(&(process->apparatStack), &temp);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    (*numOfString)++;

    return 0;
}

int PopReg(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;
    int temp = 0;
    int result = StackPop(&(process->apparatStack), &temp);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    process->registers[process->code[process->pc]] = temp;
    (process->pc)++;
    (*numOfString)++;

    return 0;
}

int Out(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;
    int gotNumber = 0;
    int result = StackGetElem(&(process->apparatStack), &gotNumber);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    fprintf(output, "%d ", gotNumber);
    (*numOfString)++;

    return 0;
}

int Add(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp1) || StackPop(&(process->apparatStack), &temp2);
    result = result || StackPush(&(process->apparatStack), temp1 + temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    (*numOfString)++;

    return 0;
}

int Sub(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp1) || StackPop(&(process->apparatStack), &temp2);
    result = result || StackPush(&(process->apparatStack), temp2 - temp1);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    (*numOfString)++;

    return 0;
}

int Mul(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp1) || StackPop(&(process->apparatStack), &temp2);
    result = result || StackPush(&(process->apparatStack), temp1 * temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    (*numOfString)++;

    return 0;
}

int Div(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp1) || StackPop(&(process->apparatStack), &temp2);

    if (temp1 == 0)
    {
        fprintf(logfileProc, "ERROR: Division by zero on line %d\n", *numOfString);
        return 1;
    }

    result = result || StackPush(&(process->apparatStack), temp2 / temp1);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    (*numOfString)++;

    return 0;
}

int Mod(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp1) || StackPop(&(process->apparatStack), &temp2);

    if (temp1 == 0)
    {
        fprintf(logfileProc, "ERROR: Division by zero on line %d\n", *numOfString);
        return 1;
    }

    result = result || StackPush(&(process->apparatStack), temp2 % temp1);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    (*numOfString)++;

    return 0;
}

int Sqr(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    int temp = 0;
    int result = StackPop(&(process->apparatStack), &temp);

    if (temp < 0)
    {
        fprintf(logfileProc, "ERROR: Square root of a negative number on line %d\n", *numOfString);
        return 1;
    }

    result = result || StackPush(&(process->apparatStack), (int)lround(sqrtl((long double)temp)));

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    (*numOfString)++;

    return 0;
}

int Jmp(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] > process->cntOfLines)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent line on line %d\n", *numOfString);
        return 1;
    }
    (process->pc)++;

    *numOfString = process->code[process->pc];
    process->pc = process->whereStartEveryString[process->code[process->pc]];
    return 0;
}

int Jb(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] > process->cntOfLines)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent line on line %d\n", *numOfString);
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp2) || StackPop(&(process->apparatStack), &temp1);
    result = result || StackPush(&(process->apparatStack), temp1);
    result = result || StackPush(&(process->apparatStack), temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    if (temp1 < temp2)
    {
        *numOfString = process->code[process->pc];
        process->pc = process->whereStartEveryString[process->code[process->pc]];
    }
    else
    {
        (process->pc)++;
        (*numOfString)++;
    }

    return 0;
}

int Jbe(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] > process->cntOfLines)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent line on line %d\n", *numOfString);
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp2) || StackPop(&(process->apparatStack), &temp1);
    result = result || StackPush(&(process->apparatStack), temp1);
    result = result || StackPush(&(process->apparatStack), temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    if (temp1 <= temp2)
    {
        *numOfString = process->code[process->pc];
        process->pc = process->whereStartEveryString[process->code[process->pc]];
    }
    else
    {
        (process->pc)++;
        (*numOfString)++;
    }

    return 0;
}

int Ja(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] > process->cntOfLines)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent line on line %d\n", *numOfString);
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp2) || StackPop(&(process->apparatStack), &temp1);
    result = result || StackPush(&(process->apparatStack), temp1);
    result = result || StackPush(&(process->apparatStack), temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    if (temp1 > temp2)
    {
        *numOfString = process->code[process->pc];
        process->pc = process->whereStartEveryString[process->code[process->pc]];
    }
    else
    {
        (process->pc)++;
        (*numOfString)++;
    }

    return 0;
}

int Jae(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] > process->cntOfLines)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent line on line %d\n", *numOfString);
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp2) || StackPop(&(process->apparatStack), &temp1);
    result = result || StackPush(&(process->apparatStack), temp1);
    result = result || StackPush(&(process->apparatStack), temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    if (temp1 >= temp2)
    {
        *numOfString = process->code[process->pc];
        process->pc = process->whereStartEveryString[process->code[process->pc]];
    }
    else
    {
        (process->pc)++;
        (*numOfString)++;
    }

    return 0;
}

int Je(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] > process->cntOfLines)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent line on line %d\n", *numOfString);
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp2) || StackPop(&(process->apparatStack), &temp1);
    result = result || StackPush(&(process->apparatStack), temp1);
    result = result || StackPush(&(process->apparatStack), temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    if (temp1 == temp2)
    {
        *numOfString = process->code[process->pc];
        process->pc = process->whereStartEveryString[process->code[process->pc]];
    }
    else
    {
        (process->pc)++;
        (*numOfString)++;
    }

    return 0;
}

int Jne(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] > process->cntOfLines)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent line on line %d\n", *numOfString);
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp2) || StackPop(&(process->apparatStack), &temp1);
    result = result || StackPush(&(process->apparatStack), temp1);
    result = result || StackPush(&(process->apparatStack), temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    if (temp1 != temp2)
    {
        *numOfString = process->code[process->pc];
        process->pc = process->whereStartEveryString[process->code[process->pc]];
    }
    else
    {
        (process->pc)++;
        (*numOfString)++;
    }

    return 0;
}

int In(SPU* process, int* numOfString)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed on line %d\n", *numOfString);
        SPUDump(process, numOfString);
        return 1;
    }

    (process->pc)++;
    int temp = 0;
    scanf("%d", &temp);
    int result = StackPush(&(process->apparatStack), temp);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack on line %d\n", *numOfString);
        return result;
    }

    (*numOfString)++;

    return 0;
}





