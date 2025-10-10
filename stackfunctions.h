#ifndef STACKFUNCTIONS_HEADER
#define STACKFUNCTIONS_HEADER

int StackVerify(Stack* stk);

// The constructor must be passed a stack with data = NULL to avoid undefined behavior.
int StackCtor(Stack* stk, ssize_t newCapacity);

int StackPush(Stack* stk, Stack_t newElem);

Stack_t StackGetElem(Stack* stk, Stack_t* getElem);

int StackPop(Stack* stk, Stack_t* popElem);

int StackDtor(Stack* stk);

#ifdef HASH_DJB2

uint64_t Hash(Stack* stk);

size_t GetNUmOfHash(Stack* stk);

#endif


#endif

