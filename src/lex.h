#ifndef X__ASCALC_LEX_H__X
#define X__ASCALC_LEX_H__X
#include <stdint.h>

typedef enum {
        TOKEN_TYPE_NULL,
        TOKEN_TYPE_ADD,
        TOKEN_TYPE_SUB,
        TOKEN_TYPE_MUL,
        TOKEN_TYPE_DIV,
        TOKEN_TYPE_MOD,
        TOKEN_TYPE_BAND,
        TOKEN_TYPE_BOR,
        TOKEN_TYPE_BNOT,
        TOKEN_TYPE_SHL,
        TOKEN_TYPE_SHR,
        TOKEN_TYPE_SEGADDR,
        TOKEN_TYPE_CHS,
        TOKEN_TYPE_TERNARY,
        TOKEN_TYPE_LNOT,
        TOKEN_TYPE_NUMBER,
        TOKEN_TYPE_HEX,
        TOKEN_TYPE_PRINT,
        TOKEN_TYPE_SECTORS,
        TOKEN_TYPE_HEADS,
        TOKEN_TYPE_DUP,
        TOKEN_TYPE_EXIT
} TOKEN_TYPE;

typedef struct {
        TOKEN_TYPE *types;
        int64_t    *values;
        uint32_t    length;
} TOKENS;

TOKEN_TYPE  chr_identify_token_type(char token);
TOKEN_TYPE  str_identify_token_type(const char *token);
int64_t     str_identify_token_value(const char *token);
const char *token_type_string(TOKEN_TYPE type);

void tokens_create(TOKENS *ret);
void tokens_destroy(TOKENS *ret);
void tokens_append(TOKENS *ret, TOKEN_TYPE type, int64_t value);
void tokens_repr(const TOKENS *ret);

void lex(TOKENS *ret, const char *source);

#endif
