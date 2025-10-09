#ifndef STACKFUNCTIONS_HEADER
#define STACKFUNCTIONS_HEADER

void StackDump(Stack* stk, const char* file, const char* func, int line);

int StackVerify(Stack* stk);

// The constructor must be passed a stack with data = NULL to avoid undefined behavior.
int StackCtor(Stack* stk, ssize_t newCapacity);

int StackPush(Stack* stk, Stack_t newElem);

Stack_t StackGetElem(Stack* stk);

int StackPop(Stack* stk, Stack_t* popElem);

int StackDtor(Stack* stk);

#ifdef HASH_DJB2

uint64_t Hash(Stack* stk);

size_t GetNUmOfHash(Stack* stk);

#endif


#endif

