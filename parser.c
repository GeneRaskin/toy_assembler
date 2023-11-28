#include "parser.h"
#include "tokens.h"
#include "env.h"
#include <string.h>

static void parse_comment(env_t *env) {
    advance(env);
    while (match(COMMENT_TEXT, env)) {
        advance(env);
    }
}

static int match_register(env_t *env) {
    return (match(R0, env) || match(R1, env) || match(R2, env)
            || match(R3, env) || match(R4, env) || match(R5, env)
            || match(R6, env) || match(R7, env) || match(R8, env)
            || match(R9, env) || match(RA, env) || match(RB, env)
            || match(RC, env) || match(RD, env) || match(RE, env)
            || match (RF, env));
}

static uint8_t register_to_code(token_t reg) {
    return (reg - OP_LJMP - 1);
}

static void parse_instruction_format_two(env_t *env) {
    uint16_t *curr_lines_count_p = &env->assembly_lines->lines_count;
    if (*curr_lines_count_p == MAX_ASSEMBLY_LINES) {
        print_error_and_exit("max assembly lines exceeded", env);
    }
    assembly_line_t *curr_assembly_line_p = &env->assembly_lines->lines[*curr_lines_count_p];
    curr_assembly_line_p->assembly_line_type = INSTRUCTION_FMT_TWO;
    curr_assembly_line_p->line.fmt_two.opcode = env->lookahead;

    advance(env);
    if (!match_register(env)) {
        fprintf(stderr, "At line %d:\n", env->curr_file_line);
        print_error_and_exit("syntax error: register is expected", env);
    }
    curr_assembly_line_p->line.fmt_two.dest = register_to_code(env->lookahead);

    advance(env);
    if (!match(ABS_ADDRESS, env) && !match(LABEL, env)) {
        print_error_and_exit("address or label is expected", env);
    }

    if (match(ABS_ADDRESS, env)) {
        long int addr = strtol(env->curr_token, NULL, 0);
        if (addr < 0x0 || addr > 0xFF) {
            fprintf(stderr, "At line %d:\n", env->curr_file_line);
            print_error_and_exit("address out of bounds", env);
        }
        curr_assembly_line_p->line.fmt_two.addr = (addr_t) addr;
    } else if (match(LABEL, env)) {
        label_t *resolved_label_p = find_label(env->sym_table, env->curr_token + 1);
        if (resolved_label_p) {
            curr_assembly_line_p->line.fmt_two.addr = resolved_label_p->addr;
        } else {
            fprintf(stderr, "At line %d:\n", env->curr_file_line);
            print_error_and_exit("unknown label", env);
        }
    }

    advance(env);
    if (match(COMMENT_START, env)) {
        parse_comment(env);
    }
    if (!match(STRING_END, env)) {
        print_error_and_exit("end of line or comment is expected", env);
    }

    (*curr_lines_count_p)++;
}

static void parse_instruction_format_one(env_t *env) {
    uint16_t *curr_lines_count_p = &env->assembly_lines->lines_count;
    if (*curr_lines_count_p == MAX_ASSEMBLY_LINES) {
        print_error_and_exit("max assembly lines exceeded", env);
    }
    assembly_line_t *curr_assembly_line_p = &env->assembly_lines->lines[*curr_lines_count_p];
    curr_assembly_line_p->assembly_line_type = INSTRUCTION_FMT_ONE;
    curr_assembly_line_p->line.fmt_one.opcode = env->lookahead;

    advance(env);
    if (!match_register(env)) {
        fprintf(stderr, "At line %d:\n", env->curr_file_line);
        print_error_and_exit("syntax error: register is expected", env);
    }
    curr_assembly_line_p->line.fmt_one.dest = register_to_code(env->lookahead);

    advance(env);
    if (!match_register(env)) {
        fprintf(stderr, "At line %d:\n", env->curr_file_line);
        print_error_and_exit("syntax error: register is expected", env);
    }
    curr_assembly_line_p->line.fmt_one.src_s = register_to_code(env->lookahead);

    advance(env);
    if (!match_register(env)) {
        fprintf(stderr, "At line %d:\n", env->curr_file_line);
        print_error_and_exit("syntax error: register is expected", env);
    }
    curr_assembly_line_p->line.fmt_one.src_t = register_to_code(env->lookahead);

    advance(env);
    if (match(COMMENT_START, env)) {
        parse_comment(env);
    }
    if (!match(STRING_END, env)) {
        fprintf(stderr, "At line %d:\n", env->curr_file_line);
        print_error_and_exit("syntax error: end of line or a comment expected", env);
    }

    (*curr_lines_count_p)++;
}

static void parse_opcode(env_t *env) {
    // Logic to handle opcode tokens and potentially their operands
    if (env->lookahead == OP_LOAD_ADDR || env->lookahead == OP_LOAD
        || env->lookahead == OP_STORE || env->lookahead == OP_BZERO ||
        env->lookahead == OP_BPOSI || env->lookahead == OP_LJMP) {
        parse_instruction_format_two(env);
    } else {
        parse_instruction_format_one(env);
    }
}

static void parse_line_first_pass(env_t *env) {
    static uint16_t curr_addr = 0;
    env->lexer_state = 0;
    env->lookahead = NONE;
    long int offset_value = 0;

    // if empty line or a line containing only comments
    if (match(STRING_END, env) || match(COMMENT_START, env)) {
        return;
    }
    // if line is not offset and is not a symbol continue
    if (!match(SYMBOL, env) && !match(OFFSET, env)) {
        curr_addr++;
        return;
    }

    // handle case when line is offset or symbol declaration
    switch (env->lookahead) {
        case SYMBOL:
            advance(env);
            if (!match(SYMBOL_NAME, env)) {
                fprintf(stderr, "At line %d:\n", env->curr_file_line);
                print_error_and_exit("syntax error: symbol name is expected", env);
            }
            if (!add_label(env->sym_table, env->curr_token, curr_addr)) {
                fprintf(stderr, "At line %d:\n", env->curr_file_line);
                print_error_and_exit("error: max label reached", env);
            }
            advance(env);
            if (!match(STRING_END, env) && !match(COMMENT_START, env)) {
                fprintf(stderr, "At line %d:\n", env->curr_file_line);
                print_error_and_exit("syntax error: end of line or a comment expected", env);
            }
            break;
        case OFFSET:
            advance(env);
            if (match(ABS_ADDRESS, env)) {
                offset_value = strtol(env->curr_token, NULL, 0);
                if (offset_value < 0x0 || offset_value > 0xFF) {
                    fprintf(stderr, "At line %d:\n", env->curr_file_line);
                    print_error_and_exit("offset out of bounds", env);
                }
            } else {
                fprintf(stderr, "At line %d:\n", env->curr_file_line);
                print_error_and_exit("syntax error: value expected", env);
            }
            advance(env);
            if (!match(STRING_END, env) && !match(COMMENT_START, env)) {
                fprintf(stderr, "At line %d:\n", env->curr_file_line);
                print_error_and_exit("syntax error: end of line or a comment expected", env);
            }
            break;
        default:
            break;
    }

    curr_addr += offset_value;
}

static void parse_offset(env_t *env) {
    uint16_t *curr_lines_count_p = &env->assembly_lines->lines_count;
    if (*curr_lines_count_p == MAX_ASSEMBLY_LINES) {
        print_error_and_exit("max assembly lines exceeded", env);
    }
    assembly_line_t *curr_assembly_line_p = &env->assembly_lines->lines[*curr_lines_count_p];
    curr_assembly_line_p->assembly_line_type = OFFSET_FMT;

    advance(env);
    if (!match(ABS_ADDRESS, env)) {
        fprintf(stderr, "At line %d:\n", env->curr_file_line);
        print_error_and_exit("syntax error: expected value", env);
    }
    curr_assembly_line_p->line.plain_value = strtol(env->curr_token, NULL, 0);

    advance(env);
    if (match(COMMENT_START, env)) {
        parse_comment(env);
    }
    if (!match(STRING_END, env)) {
        fprintf(stderr, "At line %d:\n", env->curr_file_line);
        print_error_and_exit("syntax error: end of line or comment is expected", env);
    }

    (*curr_lines_count_p)++;
}

static void parse_value(env_t *env) {
    uint16_t *curr_lines_count_p = &env->assembly_lines->lines_count;
    if (*curr_lines_count_p == MAX_ASSEMBLY_LINES) {
        print_error_and_exit("max assembly lines exceeded", env);
    }
    assembly_line_t *curr_assembly_line_p = &env->assembly_lines->lines[*curr_lines_count_p];
    curr_assembly_line_p->assembly_line_type = VALUE_FMT;

    advance(env);
    if (!match(ABS_ADDRESS, env)) {
        fprintf(stderr, "At line %d:\n", env->curr_file_line);
        print_error_and_exit("syntax error: expected value", env);
    }
    long int value = strtol(env->curr_token, NULL, 0);
    if (value > 0xFFFF || value < 0x0) {
        fprintf(stderr, "At line %d:\n", env->curr_file_line);
        print_error_and_exit("value is out of bounds", env);
    }
    curr_assembly_line_p->line.plain_value = value;

    advance(env);
    if (match(COMMENT_START, env)) {
        parse_comment(env);
    }
    if (!match(STRING_END, env)) {
        fprintf(stderr, "At line %d:\n", env->curr_file_line);
        print_error_and_exit("syntax error: end of line or comment is expected", env);
    }

    (*curr_lines_count_p)++;
}

static void parse_line_second_pass(env_t *env) {
    env->lexer_state = 0;
    env->lookahead = NONE;

    while (!match(UNKNOWN_TOKEN, env)
        && !match(STRING_END, env)) {

        switch (env->lookahead) {
            case COMMENT_START:
                parse_comment(env);
                break;
            case OP_HALT:
            case OP_ADD:
            case OP_SUBTRACT:
            case OP_AND:
            case OP_XOR:
            case OP_RSHIFT:
            case OP_LSHIFT:
            case OP_LOAD_ADDR:
            case OP_LOAD:
            case OP_STORE:
            case OP_ISTORE:
            case OP_ILOAD:
            case OP_BZERO:
            case OP_BPOSI:
            case OP_JMP:
            case OP_LJMP:
                parse_opcode(env);
                break;
            case VALUE:
                parse_value(env);
                break;
            case OFFSET:
                parse_offset(env);
                break;
            case SYMBOL_NAME:
            case SYMBOL:
                break;
            default:
                fprintf(stderr, "At line %d:\n", env->curr_file_line);
                print_error_and_exit("Syntax error: expected comment, instruction identifier,"
                                     " value, offset or symbol", env);
        }

        if (!match(UNKNOWN_TOKEN, env)) {
            advance(env);
        }

    }

    if (match(UNKNOWN_TOKEN, env)) {
        fprintf(stderr, "At line %d:\n", env->curr_file_line);
        print_error_and_exit("Unknown token", env);
    }

}

void parse_file(env_t *env) {
    char line[100];
    env->curr_file_line = 0;

    // first pass: find labels and add them to the symbol table
    while (fgets(line, sizeof(line), env->input_file_handle)) {
        env->curr_file_line++;
        env->curr_line = line;
        size_t line_len = strlen(line);
        if (line[line_len - 1] == '\n') {
            line[line_len - 1] = '\0';
        }
        parse_line_first_pass(env);
    }

    if (ferror(env->input_file_handle)) {
        print_error_and_exit("error while reading input file", env);
    }

    if (fseek(env->input_file_handle, 0, SEEK_SET) == -1) {
        print_perror_and_exit("failed to move file pointer", env);
    }
    // second pass: analyze assembly code and translate into assembly lines
    env->curr_file_line = 0;
    while (fgets(line, sizeof(line), env->input_file_handle)) {
        env->curr_file_line++;
        env->curr_line = line;
        size_t line_len = strlen(line);
        if (line[line_len - 1] == '\n') {
            line[line_len - 1] = '\0';
        }
        parse_line_second_pass(env);
    }

    if (ferror(env->input_file_handle)) {
        print_error_and_exit("error while reading input file", env);
    }
}