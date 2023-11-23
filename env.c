#include "env.h"

void free_env(env_t *env) {
    if (env) {
        if (env->assembly_lines) {
            free(env->assembly_lines);
        }
        if (env->sym_table) {
            free(env->sym_table);
        }
        if (env->input_file_handle) {
            fclose(env->input_file_handle);
        }
        if (env->output_file_handle) {
            fclose(env->output_file_handle);
        }
        free(env);
    }
}

void print_perror_and_exit(const char *message, env_t *env) {
    free_env(env);

    perror(message);
    exit(EXIT_FAILURE);
}

void print_error_and_exit(const char *message, env_t *env) {
    free_env(env);

    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}
