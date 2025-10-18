#ifndef COMMANDS_HEADER
#define COMMANDS_HEADER

#include "consts.h"

const int VERSION = 6;

const int CNT_OF_COMMANDS = 24;

enum CodeOfCommand
{
    CMD_HLT,
    CMD_PUSH,
    CMD_PUSHREG,
    CMD_POP,
    CMD_POPREG,
    CMD_OUT,
    CMD_ADD,
    CMD_SUB,
    CMD_MUL,
    CMD_DIV,
    CMD_MOD,
    CMD_SQR,
    CMD_JMP,
    CMD_JB,
    CMD_JBE,
    CMD_JA,
    CMD_JAE,
    CMD_JE,
    CMD_JNE,
    CMD_IN,
    CMD_DMP,
    CMD_CALL,
    CMD_RET,
    CMD_OUTC
};

enum TypeOfArg
{
    PARAM_ZERO,
    PARAM_NUMBER,
    PARAM_REGISTER,
    PARAM_LABEL
};

struct Command
{
    char            name[MAX_SIZE_OF_COMMAND];
    TypeOfArg       type;
    CodeOfCommand   code;
    int             (func*)(Spu*, int);
};

Command commands[] = {
    {"hlt",     PARAM_ZERO,     CMD_HLT,     SpuHlt},
    {"push",    PARAM_NUMBER,   CMD_PUSH,    SpuPush},
    {"pushreg", PARAM_REGISTER, CMD_PUSHREG, SpuPushreg},
    {"pop",     PARAM_ZERO,     CMD_POP,     SpuPop},
    {"popreg",  PARAM_REGISTER, CMD_POPREG,  SpuPopreg},
    {"out",     PARAM_ZERO,     CMD_OUT,     SpuOut},
    {"add",     PARAM_ZERO,     CMD_ADD,     SpuAdd},
    {"sub",     PARAM_ZERO,     CMD_SUB,     SpuSub},
    {"mul",     PARAM_ZERO,     CMD_MUL,     SpuMul},
    {"div",     PARAM_ZERO,     CMD_DIV,     SpuDiv},
    {"mod",     PARAM_ZERO,     CMD_MOD,     SpuMod},
    {"sqr",     PARAM_ZERO,     CMD_SQR,     SpuSqr},
    {"jmp",     PARAM_LABEL,    CMD_JMP,     SpuJmp},
    {"jb",      PARAM_LABEL,    CMD_JB,      SpuJb},
    {"jbe",     PARAM_LABEL,    CMD_JBE,     SpuJbe},
    {"ja",      PARAM_LABEL,    CMD_JA,      SpuJa},
    {"jae",     PARAM_LABEL,    CMD_JAE,     SpuJae},
    {"je",      PARAM_LABEL,    CMD_JE,      SpuJe},
    {"jne",     PARAM_LABEL,    CMD_JNE,     SpuJne},
    {"in",      PARAM_ZERO,     CMD_IN,      SpuIn},
    {"dmp",     PARAM_ZERO,     CMD_DMP,     SpuDmp},
    {"call",    PARAM_LABEL,    CMD_CALL,    SpuCall},
    {"ret",     PARAM_ZERO,     CMD_RET,     SpuRet},
    {"outc",    PARAM_ZERO,     CMD_OUTC,    SpuOutc}
};

#endif
