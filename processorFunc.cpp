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

    int result = 0;
    while ((result = DoCommand(process)) == 0)
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

    if (process->pc >= process->lengthOfCode)
    {
        errors |= BAD_PC;
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
    memset(process->registers, 0, sizeof(int) * CNT_OF_REGISTERS);

    return 0;
}

int DoCommand(SPU* process)
{
    //printf("%zu ", process->pc);
    switch(process->code[process->pc])
    {
        case HLT:
            return 1;
        case PUSH:
            return Push(process);
        case PUSHREG:
            return PushReg(process);
        case POP:
            return Pop(process);
        case POPREG:
            return PopReg(process);
        case IN:
            return In(process);
        case DMP:
            SPUDump(process);
            (process->pc)++;
            return 0;
        case OUT:
            return Out(process);
        case ADD:
            return Add(process);
        case SUB:
            return Sub(process);
        case MUL:
            return Mul(process);
        case DIV:
            return Div(process);
        case MOD:
            return Mod(process);
        case SQR:
            return Sqr(process);
        case JMP:
            return Jmp(process);
        case JB:
            return Jb(process);
        case JBE:
            return Jbe(process);
        case JA:
            return Ja(process);
        case JAE:
            return Jae(process);
        case JE:
            return Je(process);
        case JNE:
            return Jne(process);
        default:
            return 1;
    }
}

// Basic commands

int Push(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"push\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;
    int result = StackPush(&(process->apparatStack), process->code[process->pc]);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"push\".\n");
        return result;
    }

    (process->pc)++;

    return 0;
}

int PushReg(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"pushreg\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;
    int result = StackPush(&(process->apparatStack), process->registers[process->code[process->pc]]);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"pushreg\".\n");
        return result;
    }

    (process->pc)++;

    return 0;
}

int Pop(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"pop\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;
    int temp = 0;
    int result = StackPop(&(process->apparatStack), &temp);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"pop\".\n");
        return result;
    }


    return 0;
}

int PopReg(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"popreg\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;
    int temp = 0;
    int result = StackPop(&(process->apparatStack), &temp);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"popreg\".\n");
        return result;
    }

    process->registers[process->code[process->pc]] = temp;
    (process->pc)++;

    return 0;
}

int Out(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"out\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;
    int gotNumber = 0;
    int result = StackGetElem(&(process->apparatStack), &gotNumber);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"out\".\n");
        return result;
    }

    fprintf(output, "%d ", gotNumber);

    return 0;
}

int Add(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"add\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp1) || StackPop(&(process->apparatStack), &temp2);
    result = result || StackPush(&(process->apparatStack), temp1 + temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"add\".\n");
        return result;
    }


    return 0;
}

int Sub(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"sub\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp1) || StackPop(&(process->apparatStack), &temp2);
    result = result || StackPush(&(process->apparatStack), temp2 - temp1);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"sub\".\n");
        return result;
    }


    return 0;
}

int Mul(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"mul\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp1) || StackPop(&(process->apparatStack), &temp2);
    result = result || StackPush(&(process->apparatStack), temp1 * temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"mul\".\n");
        return result;
    }


    return 0;
}

int Div(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"div\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp1) || StackPop(&(process->apparatStack), &temp2);

    if (temp1 == 0)
    {
        fprintf(logfileProc, "ERROR: Division by zero in command \"div\".\n");
        return 1;
    }

    result = result || StackPush(&(process->apparatStack), temp2 / temp1);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"div\".\n");
        return result;
    }


    return 0;
}

int Mod(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"mod\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp1) || StackPop(&(process->apparatStack), &temp2);

    if (temp1 == 0)
    {
        fprintf(logfileProc, "ERROR: Division by zero in command \"mod\".\n");
        return 1;
    }

    result = result || StackPush(&(process->apparatStack), temp2 % temp1);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"mod\".\n");
        return result;
    }


    return 0;
}

int Sqr(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"sqr\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    int temp = 0;
    int result = StackPop(&(process->apparatStack), &temp);

    if (temp < 0)
    {
        fprintf(logfileProc, "ERROR: Square root of a negative number in command \"sqr\".\n");
        return 1;
    }

    result = result || StackPush(&(process->apparatStack), (int)lround(sqrtl((long double)temp)));

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"sqr\".\n");
        return result;
    }


    return 0;
}

int Jmp(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"jmp\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] >= process->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address in command \"jmp\".\n");
        return 1;
    }
    (process->pc)++;

    process->pc = (size_t)process->code[process->pc];

    return 0;
}

int Jb(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"jb\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] >= process->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address in command \"jb\".\n");
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp2) || StackPop(&(process->apparatStack), &temp1);
    result = result || StackPush(&(process->apparatStack), temp1);
    result = result || StackPush(&(process->apparatStack), temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"jb\".\n");
        return result;
    }

    if (temp1 < temp2)
    {
        process->pc = (size_t)process->code[process->pc];
    }
    else
    {
        (process->pc)++;
    }

    return 0;
}

int Jbe(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"jbe\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] >= process->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address in command \"jbe\".\n");
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp2) || StackPop(&(process->apparatStack), &temp1);
    result = result || StackPush(&(process->apparatStack), temp1);
    result = result || StackPush(&(process->apparatStack), temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"jbe\".\n");
        return result;
    }

    if (temp1 <= temp2)
    {
        process->pc = (size_t)process->code[process->pc];
    }
    else
    {
        (process->pc)++;
    }

    return 0;
}

int Ja(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"ja\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] >= process->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address in command \"ja\".\n");
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp2) || StackPop(&(process->apparatStack), &temp1);
    result = result || StackPush(&(process->apparatStack), temp1);
    result = result || StackPush(&(process->apparatStack), temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"ja\".\n");
        return result;
    }

    if (temp1 > temp2)
    {
        process->pc = (size_t)process->code[process->pc];
    }
    else
    {
        (process->pc)++;
    }

    return 0;
}

int Jae(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"jae\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] >= process->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address in command \"jae\".\n");
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp2) || StackPop(&(process->apparatStack), &temp1);
    result = result || StackPush(&(process->apparatStack), temp1);
    result = result || StackPush(&(process->apparatStack), temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"jae\".\n");
        return result;
    }

    if (temp1 >= temp2)
    {
        process->pc = (size_t)process->code[process->pc];
    }
    else
    {
        (process->pc)++;
    }

    return 0;
}

int Je(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"je\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] >= process->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address in command \"je\".\n");
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp2) || StackPop(&(process->apparatStack), &temp1);
    result = result || StackPush(&(process->apparatStack), temp1);
    result = result || StackPush(&(process->apparatStack), temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"je\".\n");
        return result;
    }

    if (temp1 == temp2)
    {
        process->pc = (size_t)process->code[process->pc];
    }
    else
    {
        (process->pc)++;
    }

    return 0;
}

int Jne(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"jne\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;

    if (process->code[process->pc] < 0 || (size_t)process->code[process->pc] >= process->lengthOfCode)
    {
        fprintf(logfileProc, "ERROR: Attempt to switch to a non-existent address in command \"jne\".\n");
        return 1;
    }

    int temp1 = 0, temp2 = 0;
    int result = StackPop(&(process->apparatStack), &temp2) || StackPop(&(process->apparatStack), &temp1);
    result = result || StackPush(&(process->apparatStack), temp1);
    result = result || StackPush(&(process->apparatStack), temp2);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"jne\".\n");
        return result;
    }

    if (temp1 != temp2)
    {
        process->pc = (size_t)process->code[process->pc];
    }
    else
    {
        (process->pc)++;
    }

    return 0;
}

int In(SPU* process)
{
    if (SPUVerify(process))
    {
        fprintf(logfileProc, "ERROR: Invalid SPU was passed in command \"in\".\n");
        SPUDump(process);
        return 1;
    }

    (process->pc)++;
    int temp = 0;
    int result = scanf("%d", &temp);

    if (result <= 0)
    {
        fprintf(logfileProc, "ERROR: An error occurred while reading input in command \"in\".\n");
        return result;
    }

    result = StackPush(&(process->apparatStack), temp);

    if (result)
    {
        fprintf(logfileProc, "ERROR: An error occurred while handling the stack in command \"in\".\n");
        return result;
    }


    return 0;
}





