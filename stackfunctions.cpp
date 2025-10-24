#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#include "stack.h"
#include "stackfunctions.h"
#include "spu_consts.h"
#include "spu_dump_funcs.h"
#include "spu_help_funcs.h"

extern FILE* logfileProc;

int StackVerify(Stack* stk)
{
    int errors = 0;

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
        PRINT_LOG_FILE_SPU("A null pointer to stk was passed. Action canceled.\n");
        return NULL_STACK_POINTER;
    }

    if (newCapacity < 0)
    {
        PRINT_LOG_FILE_SPU("Invalid capacity transferred. Action cancelled.\n");
        return BAD_CAPACITY;
    }

    Stack_t* tempPt = (Stack_t*) calloc((size_t)newCapacity, sizeof(Stack_t));
    if (tempPt == NULL)
    {
        PRINT_LOG_FILE_SPU("An error occurred while allocating memory for stack. Action cancelled.\n");
        return NULL_DATA_POINTER;
    }

    free(stk->data);    // The constructor must be passed a stack with data = NULL to avoid undefined behavior.

    stk->data = tempPt;
    stk->size = 0;
    stk->capacity = newCapacity;

    return STACK_OK;
}

int StackPush(Stack* stk, Stack_t newElem)
{
    int errors = StackVerify(stk);
    if (errors)
    {
        PRINT_LOG_FILE_SPU("A stack containing errors was passed. Action cancelled.\n");
        StackDump(stk, __FILE__, __LINE__);
        return errors;
    }

    if (stk->size >= stk->capacity)
    {
        Stack_t* tempPt = (Stack_t*) realloc(stk->data, (2 * ((size_t)stk->capacity + 1)) * sizeof(Stack_t));
        if (tempPt == NULL)
        {
            PRINT_LOG_FILE_SPU("An out of memory condition occurred while automatically growing"
                               " the data buffer. Action cancelled.\n");
            return NULL_DATA_POINTER;
        }

        stk->data = tempPt;
        size_t i = (size_t)stk->capacity;
        stk->capacity = 2 * ((ssize_t)stk->capacity + 1);
        memset(&stk->data[i], 0, sizeof(Stack_t)*(i + 2));
    }

    stk->data[stk->size] = newElem;
    stk->size++;

    return STACK_OK;
}

Stack_t StackGetElem(Stack* stk, Stack_t* getElem)
{
    int errors = StackVerify(stk);
    if (errors)
    {
        PRINT_LOG_FILE_SPU("A stack containing errors was passed. Action cancelled.\n");
        StackDump(stk, __FILE__, __LINE__);
        return errors;
    }

    if (stk->size == 0)
    {
        PRINT_LOG_FILE_SPU("Attempt to get an element from an empty stack. Action cancelled.\n");
        return BAD_SIZE;
    }

    *getElem = stk->data[stk->size - 1];

    return STACK_OK;
}

int StackPop(Stack* stk, Stack_t* popElem)
{
    int errors = StackVerify(stk);
    if (errors)
    {
        PRINT_LOG_FILE_SPU("A stack containing errors was passed. Action cancelled.\n");
        StackDump(stk, __FILE__, __LINE__);
        return errors;
    }

    if (stk->size == 0)
    {
        PRINT_LOG_FILE_SPU("Attempt to remove an element from an empty stack. Action cancelled.\n");
        return BAD_SIZE;
    }

    *popElem = stk->data[stk->size - 1];
    stk->data[stk->size - 1] = 0;
    stk->size--;

    return STACK_OK;
}

int StackDtor(Stack* stk)
{
    int errors = StackVerify(stk);
    if (errors)
    {
        PRINT_LOG_FILE_SPU("A stack containing errors was passed. Action cancelled.\n");
        StackDump(stk, __FILE__, __LINE__);
        return errors;
    }

    free(stk->data);
    stk->data = NULL;
    stk->size = 0;
    stk->capacity = 0;

    return STACK_OK;
}
