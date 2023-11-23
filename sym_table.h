#ifndef SYM_TABLE_H
#define SYM_TABLE_H

#define MAX_LABELS 100
#define MAX_LABEL_LEN 20

#include "instruction_format.h"

// Struct to hold label information
typedef struct label_s {
    char label_name[MAX_LABEL_LEN + 1];
    addr_t addr;
} label_t;

// Struct to hold symbol table
typedef struct sym_table_s {
    label_t labels[MAX_LABELS];
    uint8_t label_count;
} sym_table_t;

// Function to add a label to the symbol table
int add_label(sym_table_t *sym_table, const char *label_name, addr_t addr);

// Function to resolve a label
label_t *find_label(sym_table_t *sym_table, const char *label_name);

#endif
