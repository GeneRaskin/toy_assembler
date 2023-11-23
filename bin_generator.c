#include "env.h"
#include <stdio.h>

static void write_assembly_line(FILE *file, const assembly_line_t *line) {

    uint16_t zero_word = 0;

    switch (line->assembly_line_type) {
        case INSTRUCTION_FMT_ONE:
            fwrite(&(line->line.fmt_one), sizeof(instruction_fmt_one_t), 1, file);
            break;

        case INSTRUCTION_FMT_TWO:
            fwrite(&(line->line.fmt_two), sizeof(instruction_fmt_two_t), 1, file);
            break;

        case OFFSET_FMT:
            for (int i = 0; i < line->line.plain_value; ++i) {
                fwrite(&(zero_word), sizeof(uint16_t), 1, file);
            }
            break;

        case VALUE_FMT:
            fwrite(&(line->line.plain_value), sizeof(uint16_t), 1, file);
            break;

    }

}

// Function to generate a binary file from assembly lines
void generate_binary_file(env_t *env) {

    for (uint16_t i = 0; i < env->assembly_lines->lines_count; i++) {
        write_assembly_line(env->output_file_handle, &(env->assembly_lines->lines[i]));
        if (ferror(env->output_file_handle)) {
            print_perror_and_exit("error while generating binary", env);
        }
    }

}