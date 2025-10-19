#include <stdio.h>

#include "stack.h"
#include "stackfunctions.h"
#include "spu_consts.h"
#include "spu_dump_funcs.h"
#include "spu_structs.h"
#include "spu_general_funcs.h"

extern FILE* logfileProc;

void StackDump(Stack* stk, const char* file, const char* func, int line)
{
    int errors = StackVerify(stk);
    fprintf(logfileProc, "\nStackDump from %s at %s:%d\n"
                "stack [%p]\n", func, file, line, stk);

    if (errors & NULL_STACK_POINTER)
    {
        fprintf(logfileProc, "There is no stack at all. Oops.\n");
        fflush(logfileProc);
        return;
    }

    fprintf(logfileProc, "{\n"
                     "   capacity = %zd ", stk->capacity);
    if (errors & BAD_CAPACITY) {
        fprintf(logfileProc, "(BAD_CAPACITY)\n");
    } else {
        fprintf(logfileProc, "\n");
    }

    fprintf(logfileProc, "   size = %zd ", stk->size);
    if (errors & BAD_SIZE) {
        fprintf(logfileProc, "(BAD_SIZE)\n");
    } else {
        fprintf(logfileProc, "\n");
    }

    fprintf(logfileProc, "   data[%p] ", stk->data);
    if (errors & NULL_DATA_POINTER) {
        fprintf(logfileProc, "(NULL_DATA_POINTER)\n"
                         "}\n");
        fflush(logfileProc);
        return;
    } else {
        fprintf(logfileProc, "\n");
    }

    bool isCapacityValid = !(errors & BAD_CAPACITY);
    bool isSizeValid     = !(errors & BAD_SIZE);

    if (isCapacityValid)
    {
        fprintf(logfileProc, "   {\n");
        for (ssize_t i = 0; i < stk->capacity; i++)
        {
            if (isSizeValid && i < stk->size)
            {
                fprintf(logfileProc, "      *");
            } else {
                fprintf(logfileProc, "       ");
            }
            fprintf(logfileProc, "[%zd] = ", i);
            fprintf(logfileProc, "%d\n", stk->data[i]);
        }

        fprintf(logfileProc, "   }\n"
                         "}\n");

        if (isCapacityValid && isSizeValid)
        {
            fprintf(logfileProc, "No errors found\n");
        }
    }

    fflush(logfileProc);
}

int SpuDump(Spu* spu, int codeOfCommand)
{
    int errors = SpuVerify(spu);

    fprintf(logfileProc, "\nProcessor dump for SPU [%p] from command [%d]\n", spu, codeOfCommand);
    if (errors & NULL_SPU_POINTER)
    {
        fprintf(logfileProc, "(BAD_SPU)\n");
        return 1;
    }

    fprintf(logfileProc, "pc = %zu ", spu->pc);
    if (errors & BAD_PC)
    {
        fprintf(logfileProc, "(BAD_PC)\n");
    }
    else
    {
        fprintf(logfileProc, "\n");
    }

    if (errors & NULL_CODE_POINTER)
    {
        fprintf(logfileProc, "(BAD_CODE)\n");
    }
    else if (!(errors & BAD_PC))
    {
        SpuPrintCode(spu);
    }

    if (errors & BAD_APPARAT_STACK)
    {
        fprintf(logfileProc, "(BAD_STACK)\n");
    }
    else
    {
        SpuPrintApparatStack(spu);
    }

    SpuPrintRegisters(spu);

    if (errors & BAD_STACK_OF_RETURN_REGISTERS)
    {
        fprintf(logfileProc, "(BAD_STACK_OF_RETURN_REGISTERS)\n");
    }
    else
    {
        SpuPrintReturnRegisters(spu);
    }

    return 0;
}

void SpuPrintCode(Spu* spu)
{
    fprintf(logfileProc, "Code: ");

    if (spu->pc <= 3)
    {
        for (int i = 0; i <= 3; i++)
        {
            fprintf(logfileProc, "%.4X ", (unsigned int)spu->code[i]);
        }
        fprintf(logfileProc, "\n      %*c^^^^\n", 5 * (int)spu->pc, ' ');
    }
    else
    {
        for (size_t i = spu->pc; i <= 3 + spu->pc; i++)
        {
            fprintf(logfileProc, "%.4X ", (unsigned int)spu->code[i]);
        }
        fprintf(logfileProc, "\n                     ^^^^\n");
    }
}

void SpuPrintApparatStack(Spu* spu)
{
    fprintf(logfileProc, "Stack: ");

    if (spu->apparatStack.size == 0)
    {
        fprintf(logfileProc, "empty\n");
    }
    else
    {
        for (int i = 0; i < spu->apparatStack.size; i++)
        {
            fprintf(logfileProc, "%d ", spu->apparatStack.data[i]);
        }
        fprintf(logfileProc, "\n");
    }
}

void SpuPrintRegisters(Spu* spu)
{
    fprintf(logfileProc, "Registers: ");
    for (int i = 0; i < CNT_OF_REGISTERS; i++)
    {
        fprintf(logfileProc, "%cX = %d; ", i + 65, spu->registers[i]);
    }
    fprintf(logfileProc, "\n");
}

void SpuPrintReturnRegisters(Spu* spu)
{
    fprintf(logfileProc, "Return registers: ");

    if (spu->returnRegisters.size == 0)
    {
        fprintf(logfileProc, "empty\n");
    }
    else
    {
        for (int i = 0; i < spu->returnRegisters.size; i++)
        {
            fprintf(logfileProc, "%d ", spu->returnRegisters.data[i]);
        }
        fprintf(logfileProc, "\n");
    }
}
