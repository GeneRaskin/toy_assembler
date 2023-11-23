#ifndef INSTRUCTION_FORMAT_H
#define INSTRUCTION_FORMAT_H

#include <stdint.h>

typedef uint8_t addr_t;

#define MAX_ASSEMBLY_LINES 600

typedef struct instruction_fmt_one_s {
    uint8_t src_t:4;
    uint8_t src_s:4;
    uint8_t dest:4;
    uint8_t opcode:4;
} instruction_fmt_one_t;

typedef struct instruction_fmt_two_s {
    addr_t addr;
    uint8_t dest:4;
    uint8_t opcode:4;
} instruction_fmt_two_t;

typedef enum assembly_line_type_e {
    INSTRUCTION_FMT_ONE,
    INSTRUCTION_FMT_TWO,
    VALUE_FMT,
    OFFSET_FMT
} assembly_line_type_t;

typedef struct assembly_line_s {
    union line_u {
        struct instruction_fmt_one_s fmt_one;
        struct instruction_fmt_two_s fmt_two;
        uint16_t plain_value;
    } line;
    assembly_line_type_t assembly_line_type;
} assembly_line_t;

typedef struct assembly_lines_s {
    assembly_line_t lines[MAX_ASSEMBLY_LINES];
    uint16_t lines_count;
} assembly_lines_t;

#endif
