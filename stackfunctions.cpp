#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#include "stack.h"
#include "helpfunc.h"
#include "stackfunctions.h"
#include "consts.h"

#ifdef HASH_DJB2
extern uint64_t hashes[MAX_COUNT_OF_HASHES];
extern size_t cntOfHashes;
#endif

extern FILE* logfileStack;

void StackDump(Stack* stk, const char* file, const char* func, int line)
{
    int errors = StackVerify(stk);
    fprintf(logfileStack, "\nStackDump from %s at %s:%d\n"
                "stack [%p]\n", func, file, line, stk);

#if defined(CANARY_PROTECT) || defined(HASH_DJB2)
    if (errors & DAMAGED_STACK)
    {
        fprintf(logfileStack, "The protection was triggered. The stack is damaged.\n");
        fflush(logfileStack);
        return;
    }
#endif
    if (errors & NULL_STACK_POINTER)
    {
        fprintf(logfileStack, "There is no stack at all. Oops.\n");
        fflush(logfileStack);
        return;
    }

    fprintf(logfileStack, "{\n"
                     "   capacity = %zd ", stk->capacity);
    if (errors & BAD_CAPACITY) {
        fprintf(logfileStack, "(BAD_CAPACITY)\n");
    } else {
        fprintf(logfileStack, "\n");
    }

    fprintf(logfileStack, "   size = %zd ", stk->size);
    if (errors & BAD_SIZE) {
        fprintf(logfileStack, "(BAD_SIZE)\n");
    } else {
        fprintf(logfileStack, "\n");
    }

    fprintf(logfileStack, "   data[%p] ", stk->data);
    if (errors & NULL_DATA_POINTER) {
        fprintf(logfileStack, "(NULL_DATA_POINTER)\n"
                         "}\n");
        fflush(logfileStack);
        return;
    } else {
        fprintf(logfileStack, "\n");
    }

    bool isCapacityValid = !(errors & BAD_CAPACITY);
    bool isSizeValid     = !(errors & BAD_SIZE);

    if (isCapacityValid)
    {
        fprintf(logfileStack, "   {\n");
        for (ssize_t i = 0; i < stk->capacity; i++)
        {
            if (isSizeValid && i < stk->size)
            {
                fprintf(logfileStack, "      *");
            } else {
                fprintf(logfileStack, "       ");
            }
            fprintf(logfileStack, "[%zd] = ", i);
            FprintfStack_t(logfileStack, stk->data[i], "\n");
        }

        fprintf(logfileStack, "   }\n"
                         "}\n");

        if (isCapacityValid && isSizeValid)
        {
            fprintf(logfileStack, "No errors found\n");
        }
    }

    fflush(logfileStack);
}

int StackVerify(Stack* stk)
{
    int errors = 0;

#ifdef CANARY_PROTECT
    if (*(uint64_t*)(stk) != '!@meow#!' || *(uint64_t*)((char*)stk + sizeof(Stack) - 8) != '!@meow#!')
    {
        return DAMAGED_STACK;
    }
#endif

#ifdef HASH_DJB2
    size_t num = 0;
    num = GetNUmOfHash(stk);
    if (num >= cntOfHashes || Hash(stk) != hashes[num])
    {
        return DAMAGED_STACK;
    }
#endif

    if (stk == NULL)
    {
        return NULL_STACK_POINTER;
    }

    if (stk->capacity < 0)
    {
        errors |= BAD_CAPACITY;
    }

    if (stk->size < 0 || stk->size > stk->capacity)
    {
        errors |= BAD_SIZE;
    }

    if (stk->data == NULL)
    {
        errors |= BAD_SIZE;
        return errors;
    }

    return errors;
}

int StackCtor(Stack* stk, ssize_t newCapacity)
{
    if (stk == NULL)
    {
        PrintErrorInfo(__FILE__, __func__, __LINE__, "A null pointer to stk was passed. Action canceled.\n");
        return NULL_STACK_POINTER;
    }

    if (newCapacity < 0)
    {
        PrintErrorInfo(__FILE__, __func__, __LINE__, "Invalid capacity transferred. Action cancelled.\n");
        return BAD_CAPACITY;
    }

    // Смысловое содержание функции
    Stack_t* tempPt = (Stack_t*) calloc((size_t)newCapacity, sizeof(Stack_t));
    if (tempPt == NULL)
    {
        PrintErrorInfo(__FILE__, __func__, __LINE__, "An error occurred while allocating memory for stack. Action cancelled.\n");
        return NULL_DATA_POINTER;
    }

    free(stk->data);    // The constructor must be passed a stack with data = NULL to avoid undefined behavior.

    stk->data = tempPt;
    stk->size = 0;
    stk->capacity = newCapacity;

#ifdef CANARY_PROTECT
    stk->canary1 = '!@meow#!';         // Canary protection
    stk->canary2 = '!@meow#!';
#endif

#ifdef HASH_DJB2
    stk->numOfHash = cntOfHashes;
    hashes[cntOfHashes] = Hash(stk);
    cntOfHashes++;
#endif
    //
    return STACK_OK;
}

int StackPush(Stack* stk, Stack_t newElem)
{
    int errors = StackVerify(stk);
    if (errors)
    {
        PrintErrorInfo(__FILE__, __func__, __LINE__, "A stack containing errors was passed. Action cancelled.\n");
        StackDump(stk, __FILE__, __func__, __LINE__);
        return errors;
    }

    // Смысловое содержание функции
    if (stk->size >= stk->capacity)
    {
        Stack_t* tempPt = (Stack_t*) realloc(stk->data, (2 * ((size_t)stk->capacity + 1)) * sizeof(Stack_t));
        if (tempPt == NULL)
        {
            PrintErrorInfo(__FILE__, __func__, __LINE__, "An out of memory condition occurred while automatically growing the data buffer. Action cancelled.\n");
            return NULL_DATA_POINTER;
        }

        stk->data = tempPt;
        size_t i = (size_t)stk->capacity;
        stk->capacity = 2 * ((ssize_t)stk->capacity + 1);
        memset(&stk->data[i], 0, sizeof(Stack_t)*(i + 2));
    }

    stk->data[stk->size] = newElem;
    stk->size++;

#ifdef HASH_DJB2
    hashes[GetNUmOfHash(stk)] = Hash(stk);
#endif
    //

    return STACK_OK;
}

Stack_t StackGetElem(Stack* stk)
{
    int errors = StackVerify(stk);
    if (errors)
    {
        PrintErrorInfo(__FILE__, __func__, __LINE__, "A stack containing errors was passed. Action cancelled.\n");
        StackDump(stk, __FILE__, __func__, __LINE__);
        return INT_MAX;
    }

    if (stk->size == 0)
    {
        PrintErrorInfo(__FILE__, __func__, __LINE__, "Attempting to get an element from an empty stack. Action cancelled.\n");
        return INT_MAX;
    }

    return stk->data[stk->size - 1];
}

int StackPop(Stack* stk, Stack_t* popElem)
{
    int errors = StackVerify(stk);
    if (errors)
    {
        PrintErrorInfo(__FILE__, __func__, __LINE__, "A stack containing errors was passed. Action cancelled.\n");
        StackDump(stk, __FILE__, __func__, __LINE__);
        return errors;
    }

    // Смысловое содержание функции
    if (stk->size == 0)
    {
        PrintErrorInfo(__FILE__, __func__, __LINE__, "Attempt to remove an element from an empty stack. Action cancelled.\n");
        return BAD_SIZE;
    }

    *popElem = stk->data[stk->size - 1];
    stk->data[stk->size - 1] = 0;
    stk->size--;

#ifdef HASH_DJB2
    hashes[GetNUmOfHash(stk)] = Hash(stk);
#endif
    //

    return STACK_OK;
}

int StackDtor(Stack* stk)
{
    int errors = StackVerify(stk);
    if (errors)
    {
        PrintErrorInfo(__FILE__, __func__, __LINE__, "A stack containing errors was passed. Action cancelled.\n");
        StackDump(stk, __FILE__, __func__, __LINE__);
        return errors;
    }

    // Смысловое содержание функции
    free(stk->data);
    stk->data = NULL;
    stk->size = 0;
    stk->capacity = 0;

#ifdef CANARY_PROTECT
    stk->canary1 = 0;
    stk->canary2 = 0;
#endif

#ifdef HASH_DJB2
    hash[GetNUmOfHash(stk)] = 0;
    stk->numOfHash = 0;
#endif
    //
    return STACK_OK;
}

#ifdef HASH_DJB2

uint64_t Hash(Stack* stk)
{
    uint64_t tempHash = 5381;
    char* p1 = (char*)stk;
    char* p2 = (char*)stk + sizeof(Stack) - 1;

    for (int i = 0; p1 + i <= p2; i++)
    {
        tempHash = ((tempHash << 5) - tempHash) + (uint64_t)(*(p1 + i));
    }

    return tempHash;
}

size_t GetNUmOfHash(Stack* stk)
{
#ifdef CANARY_PROTECT
    size_t numOfHash = *(size_t*)((char*)stk + 32);
#else
    size_t numOfHash = *(size_t*)((char*)stk + 24);
#endif
    return numOfHash;
}

#endif
