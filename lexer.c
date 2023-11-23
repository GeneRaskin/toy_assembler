#include "tokens.h"
#include "env.h"
#include "instruction_format.h"
#include <string.h>
#include <errno.h>

static token_t which_register(char *curr_token) {
    if (strlen(curr_token) != 2) {
        return UNKNOWN_TOKEN;
    }
    for (token_t candidate_reg = R0; candidate_reg <= RF; candidate_reg++) {
        if (strncmp(curr_token, reg_names[candidate_reg - OP_LJMP - 1], 2) == 0
            && strlen(curr_token) == 2) {
            return candidate_reg;
        }
    }
    return UNKNOWN_TOKEN;
}

static token_t which_opcode(char *curr_token) {
    for (token_t candidate_opcode = OP_HALT; candidate_opcode <= OP_LJMP; candidate_opcode++) {
        const char *instruction_name = instruction_names[candidate_opcode];
        if (strncmp(curr_token, instruction_name, strlen(instruction_name)) == 0
            && strlen(curr_token) == strlen(instruction_name)) {
            return candidate_opcode;
        }
    }
    return UNKNOWN_TOKEN;
}

static token_t check_keywords(env_t *env) {
    if (strncmp(env->curr_token, "offset", 6) == 0
        && strlen(env->curr_token) == 6) {
        return OFFSET;
    }
    if (strncmp(env->curr_token, "sym", 3) == 0
        && strlen(env->curr_token) == 3) {
        env->lexer_state |= IS_SYMBOL;
        return SYMBOL;
    }
    if (strncmp(env->curr_token, "val", 3) == 0
        && strlen(env->curr_token) == 3) {
        return VALUE;
    }
    return UNKNOWN_TOKEN;
}

static token_t which_token(env_t *env) {
    if (env->lexer_state & IS_COMMENT) {
        return COMMENT_TEXT;
    }

    if (env->curr_token[0] == '#') {
        env->lexer_state |= IS_COMMENT;
        return COMMENT_START;
    }

    if (env->curr_token[0] == '@' && strlen(env->curr_token) > 1) {
        return LABEL;
    }

    if (env->lexer_state & IS_SYMBOL) {
        return SYMBOL_NAME;
    }

    token_t keyword_token = check_keywords(env);
    if (keyword_token != UNKNOWN_TOKEN) {
        return keyword_token;
    }

    token_t resolved_token = which_opcode(env->curr_token);
    if (resolved_token != UNKNOWN_TOKEN) {
        return resolved_token;
    }

    errno = 0;
    strtol(env->curr_token, NULL, 0);
    if (errno != EINVAL) {
        return ABS_ADDRESS;
    }

    resolved_token = which_register(env->curr_token);
    if (resolved_token != UNKNOWN_TOKEN) {
        return resolved_token;
    }

    return UNKNOWN_TOKEN;
}

static token_t lex(env_t *env) {
    char *token = strsep(&env->curr_line, " ");
    env->curr_token = token;
    if (token != NULL && *env->curr_token) {
        return which_token(env);
    }
    return STRING_END;
}

int match(token_t token, env_t *env) {
    if (env->lookahead == NONE) {
        env->lookahead = lex(env);
    }
    return (token == env->lookahead);
}

void advance(env_t *env) {
    env->lookahead = lex(env);
}
