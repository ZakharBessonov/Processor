#ifndef STACK_HEADER
#define STACK_HEADER

typedef int Stack_t;

struct Stack
{
#ifdef CANARY_PROTECT
    uint64_t        canary1;
#endif
    Stack_t*           data;
    ssize_t            size;
    ssize_t        capacity;
#ifdef HASH_DJB2
    size_t        numOfHash;
#endif

#ifdef CANARY_PROTECT
    uint64_t        canary2;
#endif
};

enum StackErr_t
{
    STACK_OK = 0,
    NULL_STACK_POINTER = 1,
    BAD_CAPACITY = 2,
    BAD_SIZE = 4,
    NULL_DATA_POINTER = 8,
#if defined(CANARY_PROTECT) || defined(HASH_DJB2)
    DAMAGED_STACK = 16
#endif
};

#endif

