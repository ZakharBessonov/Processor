#ifndef STACK_HEADER
#define STACK_HEADER

typedef int Stack_t;

struct Stack
{
    Stack_t*           data;
    ssize_t            size;
    ssize_t        capacity;
};

enum StackErr_t
{
    STACK_OK = 0,
    NULL_STACK_POINTER = 1,
    BAD_CAPACITY = 2,
    BAD_SIZE = 4,
    NULL_DATA_POINTER = 8
};

#endif

