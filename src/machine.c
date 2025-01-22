#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lex.h"
#include "machine.h"

void state_create(STATE *ret) {
        memset(ret, 0, sizeof(*ret));
        ret->stack   = malloc(1);
        ret->heads   =      0xFF;
        ret->sectors =        63;
}
void state_destroy(STATE *ret) {
        free(ret->stack);
        memset(ret, 0, sizeof(*ret));
}
void state_stack_push(STATE *ret, int64_t value) {
        ret->stack = realloc(ret->stack, ++ret->stack_length * sizeof(*ret->stack));
        ret->stack[ret->stack_length - 1] = value;
}
int64_t state_stack_pop(STATE *ret) {
        int64_t value;
        if(!ret->stack_length) {
                fprintf(stderr, "Error: Expected item on the stack for pop argument.\n");
                return 0;
        }
        value = ret->stack[ret->stack_length - 1];
        ret->stack = realloc(ret->stack, --ret->stack_length * sizeof(*ret->stack));
        return value;
}

void simulate_machine(const TOKENS *tokens, STATE *state) {
        uint64_t IP;
        int64_t a, b, c;
        uint64_t ua, ub, uc, uret;
        for(IP = 0; IP < tokens->length; IP++) {
                switch(tokens->types[IP]) {
                case TOKEN_TYPE_ADD:
                        b = state_stack_pop(state);
                        a = state_stack_pop(state);
                        state_stack_push(state, a+b);
                        break;
                case TOKEN_TYPE_SUB:
                        b = state_stack_pop(state);
                        a = state_stack_pop(state);
                        state_stack_push(state, a-b);
                        break;
                case TOKEN_TYPE_MUL:
                        b = state_stack_pop(state);
                        a = state_stack_pop(state);
                        fprintf(stderr, "%d * %d = %d\n", a, b, a*b);
                        state_stack_push(state, a*b);
                        break;
                case TOKEN_TYPE_DIV:
                        b = state_stack_pop(state);
                        a = state_stack_pop(state);
                        state_stack_push(state, a/b);
                        break;
                case TOKEN_TYPE_MOD:
                        b = state_stack_pop(state);
                        a = state_stack_pop(state);
                        state_stack_push(state, a%b);
                        break;
                case TOKEN_TYPE_BAND:
                        b = state_stack_pop(state);
                        a = state_stack_pop(state);
                        ub = *(uint64_t*)&b;
                        ua = *(uint64_t*)&a;
                        uret = ua & ub;
                        state_stack_push(state, *(int64_t*)&uret);
                        break;
                case TOKEN_TYPE_BOR:
                        b = state_stack_pop(state);
                        a = state_stack_pop(state);
                        ub = *(uint64_t*)&b;
                        ua = *(uint64_t*)&a;
                        uret = ua | ub;
                        state_stack_push(state, *(int64_t*)&uret);
                        break;
                case TOKEN_TYPE_BNOT:
                        a = state_stack_pop(state);
                        ua = *(uint64_t*)&a;
                        uret = ~ua;
                        state_stack_push(state, *(int64_t*)&uret);
                        break;
                case TOKEN_TYPE_SHL:
                        b = state_stack_pop(state);
                        a = state_stack_pop(state);
                        ub = *(uint64_t*)&b;
                        ua = *(uint64_t*)&a;
                        uret = ua << ub;
                        state_stack_push(state, *(int64_t*)&uret);
                        break;
                case TOKEN_TYPE_SHR:
                        b = state_stack_pop(state);
                        a = state_stack_pop(state);
                        ub = *(uint64_t*)&b;
                        ua = *(uint64_t*)&a;
                        uret = ua >> ub;
                        state_stack_push(state, *(int64_t*)&uret);
                        break;
                case TOKEN_TYPE_SEGADDR:
                        b = state_stack_pop(state);
                        a = state_stack_pop(state);
                        ub = *(uint64_t*)&b;
                        ua = *(uint64_t*)&a;
                        uret = ((ua & 0xFFFF) << 4) + ub;
                        state_stack_push(state, *(int64_t*)&uret);
                        break;
                case TOKEN_TYPE_CHS:
                        c = state_stack_pop(state);
                        b = state_stack_pop(state);
                        a = state_stack_pop(state);
                        uc = *(uint64_t*)&c;
                        ub = *(uint64_t*)&b;
                        ua = *(uint64_t*)&a;
                        uret = (a * state->heads + b) * state->sectors + (c - 1);
                        state_stack_push(state, *(int64_t*)&uret);
                        break;
                case TOKEN_TYPE_TERNARY:
                        c = state_stack_pop(state);
                        b = state_stack_pop(state);
                        a = state_stack_pop(state);
                        state_stack_push(state, c ? a : b);
                        break;
                case TOKEN_TYPE_LNOT:
                        a = state_stack_pop(state);
                        ua = *(uint64_t*)&a;
                        uret = !ua;
                        state_stack_push(state, *(int64_t*)&uret);
                        break;
                case TOKEN_TYPE_NUMBER:
                        state_stack_push(state, tokens->values[IP]);
                        break;
                case TOKEN_TYPE_HEX:
                        printf("$%X\n", state_stack_pop(state));
                        break;
                case TOKEN_TYPE_PRINT:
                        printf("%d\n", state_stack_pop(state));
                        break;
                case TOKEN_TYPE_SECTORS:
                        state->sectors = state_stack_pop(state);
                        break;
                case TOKEN_TYPE_HEADS:
                        state->heads = state_stack_pop(state);
                        break;
                case TOKEN_TYPE_EXIT:
                        goto exit;
                }
        }
exit:
        return;
}
