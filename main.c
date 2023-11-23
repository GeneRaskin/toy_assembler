#include <stdio.h>
#include "env.h"
#include "parser.h"
#include "bin_generator.h"

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "usage: %s input_filename output_filename\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    env_t *env = (env_t *) malloc(sizeof(env_t));
    if (!env) {
        print_perror_and_exit("failed to allocate memory for env\n", NULL);
    }
    env->input_file_handle = NULL;
    env->output_file_handle = NULL;
    env->assembly_lines = NULL;
    env->sym_table = NULL;

    env->sym_table = (sym_table_t *) malloc(sizeof(sym_table_t));
    if (!env->sym_table) {
        print_perror_and_exit("failed to allocate memory for sym_table", env);
    }
    env->sym_table->label_count = 0;

    env->assembly_lines = (assembly_lines_t *) malloc(sizeof(assembly_lines_t));
    if (!env->assembly_lines) {
        print_perror_and_exit("failed to allocate memory for assembly_lines", env);
    }
    env->input_file_handle = fopen(input_filename, "r");
    if (!env->input_file_handle) {
        print_perror_and_exit("failed to open file for reading", env);
    }
    env->assembly_lines->lines_count = 0;

    parse_file(env);

    env->output_file_handle = fopen(output_filename, "w");
    if (!env->output_file_handle) {
        print_perror_and_exit("failed to open file for writing", env);
    }
    generate_binary_file(env);
    free_env(env);

    return EXIT_SUCCESS;
}
