#include "sym_table.h"
#include <string.h>

int    add_label(sym_table_t *sym_table, const char *label_name, addr_t addr) {
    uint8_t *label_count_p = &sym_table->label_count;
    if (*label_count_p == MAX_LABELS) {
        return 0;
    }
    label_t *last_label_p = &sym_table->labels[*label_count_p];
    strlcpy(last_label_p->label_name, label_name, MAX_LABEL_LEN + 1);
    last_label_p->addr = addr;
    (*label_count_p)++;
    return 1;
}

label_t *find_label(sym_table_t *sym_table, const char *label_name) {
    uint8_t label_count = sym_table->label_count;
    for (int i = 0; i < label_count; ++i) {
        if (!strncmp(sym_table->labels[i].label_name, label_name,
                    MAX_LABEL_LEN + 1)) {
            return (&sym_table->labels[i]);
        }
    }
    return NULL;
}
