#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lex.h"

TOKEN_TYPE chr_identify_token_type(char token) {
        switch(token) {
        case '.':       return     TOKEN_TYPE_DUP;
        case ',':       return     TOKEN_TYPE_POP;
        case '+':       return     TOKEN_TYPE_ADD;
        case '-':       return     TOKEN_TYPE_SUB;
        case '*':       return     TOKEN_TYPE_MUL;
        case '/':       return     TOKEN_TYPE_DIV;
        case '%':       return     TOKEN_TYPE_MOD;
        case '@':       return     TOKEN_TYPE_CHS;
        case '|':       return     TOKEN_TYPE_BOR;
        case '<':       return     TOKEN_TYPE_SHL;
        case '>':       return     TOKEN_TYPE_SHR;
        case '&':       return    TOKEN_TYPE_BAND;
        case '~':       return    TOKEN_TYPE_BNOT;
        case '!':       return    TOKEN_TYPE_LNOT;
        case '?':       return TOKEN_TYPE_TERNARY;
        case ':':       return TOKEN_TYPE_SEGADDR;
        default:        return    TOKEN_TYPE_NULL;
        }
}
TOKEN_TYPE str_identify_token_type(const char *token) {
        char *eptr;
        if(!*token)                   return    TOKEN_TYPE_NULL;
        if(!strcmp(token, "dup"))     return     TOKEN_TYPE_DUP;
        if(!strcmp(token, "pop"))     return     TOKEN_TYPE_POP;
        if(!strcmp(token, "hex"))     return     TOKEN_TYPE_HEX;
        if(!strcmp(token, "print"))   return   TOKEN_TYPE_PRINT;
        if(!strcmp(token, "sectors")) return TOKEN_TYPE_SECTORS;
        if(!strcmp(token, "heads"))   return   TOKEN_TYPE_HEADS;
        if(!strcmp(token, "exit"))    return    TOKEN_TYPE_EXIT;
        if(!strcmp(token, "add"))     return     TOKEN_TYPE_ADD;
        if(!strcmp(token, "sub"))     return     TOKEN_TYPE_SUB;
        if(!strcmp(token, "mul"))     return     TOKEN_TYPE_MUL;
        if(!strcmp(token, "div"))     return     TOKEN_TYPE_DIV;
        if(!strcmp(token, "mod"))     return     TOKEN_TYPE_MOD;
        if(!strcmp(token, "chs"))     return     TOKEN_TYPE_CHS;
        if(!strcmp(token, "or"))      return     TOKEN_TYPE_BOR;
        if(!strcmp(token, "and"))     return    TOKEN_TYPE_BAND;
        if(!strcmp(token, "not"))     return    TOKEN_TYPE_BNOT;
        if(!strcmp(token, "shl"))     return     TOKEN_TYPE_SHL;
        if(!strcmp(token, "shr"))     return     TOKEN_TYPE_SHR;
        if(!strcmp(token, "lnot"))    return    TOKEN_TYPE_LNOT;
        if(!strcmp(token, "cond"))    return TOKEN_TYPE_TERNARY;
        if(!strcmp(token, "seg"))     return TOKEN_TYPE_SEGADDR;
        if(!strcmp(token, "label"))   return TOKEN_TYPE_LABEL;
        if(!strcmp(token, "goto"))    return TOKEN_TYPE_GOTO;
        if(!strcmp(token, "gotoif"))  return TOKEN_TYPE_GOTOIF;
        if(!strcmp(token, "get"))     return TOKEN_TYPE_GET;
        if(!strcmp(token, "set"))     return TOKEN_TYPE_SET;
        if(*token == '$')                               strtol(token+1, &eptr, 0x10);
        else if(*token == '0' && token[1] == 'x')       strtol(token+2, &eptr, 0x10);
        else                                            strtol(token,   &eptr, 0x0A);
        if(!*eptr) return TOKEN_TYPE_NUMBER;
        fprintf(stderr, "Unidentifiable token `%s`\n", token);
        return TOKEN_TYPE_NULL;
}
int64_t str_identify_token_value(const char *token) {
        char *eptr;
        int64_t value;
        if(*token == '$')                               value = strtol(token+1, &eptr, 0x10);
        else if(*token == '0' && token[1] == 'x')       value = strtol(token+2, &eptr, 0x10);
        else                                            value = strtol(token,   &eptr, 0x0A);
        return !*eptr * value;
}
const char *token_type_string(TOKEN_TYPE type) {
        switch(type) {
        case    TOKEN_TYPE_NULL: return         "null";
        case     TOKEN_TYPE_ADD: return          "add";
        case     TOKEN_TYPE_SUB: return          "sub";
        case     TOKEN_TYPE_MUL: return          "mul";
        case     TOKEN_TYPE_DIV: return          "div";
        case     TOKEN_TYPE_MOD: return          "mod";
        case    TOKEN_TYPE_BAND: return         "band";
        case     TOKEN_TYPE_BOR: return          "bor";
        case    TOKEN_TYPE_BNOT: return         "bnot";
        case     TOKEN_TYPE_SHL: return          "shl";
        case     TOKEN_TYPE_SHR: return          "shr";
        case TOKEN_TYPE_SEGADDR: return      "segaddr";
        case     TOKEN_TYPE_CHS: return          "chs";
        case TOKEN_TYPE_TERNARY: return      "ternary";
        case    TOKEN_TYPE_LNOT: return         "lnot";
        case  TOKEN_TYPE_NUMBER: return       "number";
        case     TOKEN_TYPE_HEX: return          "hex";
        case   TOKEN_TYPE_PRINT: return        "print";
        case TOKEN_TYPE_SECTORS: return      "sectors";
        case   TOKEN_TYPE_HEADS: return        "heads";
        case     TOKEN_TYPE_DUP: return          "dup";
        case    TOKEN_TYPE_EXIT: return         "exit";
        case   TOKEN_TYPE_LABEL: return        "label";
        case    TOKEN_TYPE_GOTO: return         "goto";
        case  TOKEN_TYPE_GOTOIF: return       "gotoif";
        case     TOKEN_TYPE_POP: return          "pop";
        case     TOKEN_TYPE_GET: return          "get";
        case     TOKEN_TYPE_SET: return          "set";
        default:                 return "unclassified";
        }
}

void tokens_create(TOKENS *ret) {
        ret->types  = malloc(1);
        ret->values = malloc(1);
        ret->length =         0;
}
void tokens_destroy(TOKENS *ret) {
        free(ret->types);
        free(ret->values);
        memset(ret, 0, sizeof(*ret));
}
void tokens_append(TOKENS *ret, TOKEN_TYPE type, int64_t value) {
        ret->length += 1;
        ret->types  = realloc(ret->types,  ret->length *  sizeof(*ret->types));
        ret->values = realloc(ret->values, ret->length * sizeof(*ret->values));
        ret->types[ret->length - 1]  =  type;
        ret->values[ret->length - 1] = value;
}
void tokens_repr(const TOKENS *tokens) {
        uint32_t i;
        for(i = 0; i < tokens->length; i++) {
                fprintf(stderr, "||%010d| %-15s", i, token_type_string(tokens->types[i]));
                if(tokens->types[i] == TOKEN_TYPE_NUMBER) fprintf(stderr, " : $%016X||\n", tokens->values[i]);
                else fprintf(stderr, " :  %16s||\n", " ");
        }
}

void lex(TOKENS *ret, const char *source) {
        uint32_t i;
        char *buffer;
        TOKEN_TYPE tmp_type, tmp2_type;
        buffer = calloc(1,1);
        for(i = 0; source[i]; i++) {
                if((tmp_type = chr_identify_token_type(source[i])) == TOKEN_TYPE_NULL && !isspace(source[i]) && source[i] != '\\') {
                        buffer = realloc(buffer, strlen(buffer) + 2);
                        buffer[strlen(buffer) + 1] = 0;
                        buffer[strlen(buffer)] = source[i];
                        continue;
                }
                if((tmp2_type = str_identify_token_type(buffer)) != TOKEN_TYPE_NULL) {
                        tokens_append(ret, tmp2_type, str_identify_token_value(buffer));
                }
                if(tmp_type != TOKEN_TYPE_NULL) tokens_append(ret, tmp_type, 0);
                if(source[i] == '\\') {
                        i++;
                        while(source[i] && source[i] != '\\') i++;
                        if(!source[i]) { fprintf(stderr, "Error: expected ending comment.\n"); }
                }
                *(buffer = realloc(buffer, 1)) = 0;
        }
        if((tmp2_type = str_identify_token_type(buffer)) != TOKEN_TYPE_NULL) {
                tokens_append(ret, tmp2_type, str_identify_token_value(buffer));
        }
}
