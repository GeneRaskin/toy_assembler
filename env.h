#ifndef ENV_H
#define ENV_H

#include <stdlib.h>
#include <stdio.h>
#include "tokens.h"
#include "instruction_format.h"
#include "sym_table.h"

typedef struct env_s {
    char    *curr_line;
    char    *curr_token;
    token_t lookahead;
    lexer_state_t lexer_state;
    assembly_lines_t *assembly_lines;
    sym_table_t *sym_table;
    uint16_t curr_file_line;
    FILE *input_file_handle;
    FILE *output_file_handle;
} env_t;

void print_perror_and_exit(const char *message, env_t *env);
void print_error_and_exit(const char *message, env_t *env);
void free_env(env_t *env);

#endif
