#ifndef SPU_COMMANDS_HEADER
#define SPU_COMMANDS_HEADER

#include "codes_of_commands.h"
#include "spu_funcs_for_commands.h"
#include "spu_dump_funcs.h"

struct SpuCommand
{
    char            name[MAX_SIZE_OF_COMMAND];
    CodeOfCommand   code;
    int             (*func)(Spu*, int);
};

const SpuCommand cmds[] = {
    {"hlt",     CMD_HLT,     SpuHlt             },
    {"push",    CMD_PUSH,    SpuPush            },
    {"pushreg", CMD_PUSHREG, SpuPushReg         },
    {"pop",     CMD_POP,     SpuPop             },
    {"popreg",  CMD_POPREG,  SpuPopReg          },
    {"out",     CMD_OUT,     SpuOutFunc         },
    {"add",     CMD_ADD,     SpuBasicArithmetic },
    {"sub",     CMD_SUB,     SpuBasicArithmetic },
    {"mul",     CMD_MUL,     SpuBasicArithmetic },
    {"div",     CMD_DIV,     SpuBasicArithmetic },
    {"mod",     CMD_MOD,     SpuBasicArithmetic },
    {"sqr",     CMD_SQR,     SpuSqr             },
    {"jmp",     CMD_JMP,     SpuJmp             },
    {"jb",      CMD_JB,      SpuJumpFunc        },
    {"jbe",     CMD_JBE,     SpuJumpFunc        },
    {"ja",      CMD_JA,      SpuJumpFunc        },
    {"jae",     CMD_JAE,     SpuJumpFunc        },
    {"je",      CMD_JE,      SpuJumpFunc        },
    {"jne",     CMD_JNE,     SpuJumpFunc        },
    {"in",      CMD_IN,      SpuIn              },
    {"dmp",     CMD_DMP,     SpuDump            },
    {"call",    CMD_CALL,    SpuCall            },
    {"ret",     CMD_RET,     SpuRet             },
    {"outc",    CMD_OUTC,    SpuOutFunc         },
    {"pushm",   CMD_PUSHM,   SpuPushm           },
    {"popm",    CMD_POPM,    SpuPopm            },
    {"draw",    CMD_DRAW,    SpuDraw            },
};

#endif
