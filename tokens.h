#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

typedef enum token_e {
    OP_HALT = 0x0,
    OP_ADD,
    OP_SUBTRACT,
    OP_AND,
    OP_XOR,
    OP_LSHIFT,
    OP_RSHIFT,
    OP_LOAD_ADDR,
    OP_LOAD,
    OP_STORE,
    OP_ILOAD,
    OP_ISTORE,
    OP_BZERO,
    OP_BPOSI,
    OP_JMP,
    OP_LJMP = 0xF,
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    RA,
    RB,
    RC,
    RD,
    RE,
    RF,
    COMMENT_TEXT,
    COMMENT_START,
    LABEL,
    OFFSET,
    ABS_ADDRESS,
    SYMBOL,
    SYMBOL_NAME,
    VALUE,
    STRING_END,
    UNKNOWN_TOKEN,
    NONE
} token_t;

typedef enum lexer_state_flag_e {
    IS_COMMENT = 0x1,
    IS_SYMBOL = 0x2,
} lexer_state_flag_t;

typedef uint32_t lexer_state_t;

extern const char *instruction_names[];

extern const char *reg_names[];

struct env_s;

int match(token_t token, struct env_s *env);
void advance(struct env_s *env);

#endif
