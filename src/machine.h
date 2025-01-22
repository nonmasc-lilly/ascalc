#ifndef X__ASCALC_MACHINE_H__X
#define X__ASCALC_MACHINE_H__X
#include <stdint.h>
#include "lex.h"

typedef struct {
        int64_t      *stack;
        uint64_t      stack_length;
        uint64_t      heads;
        uint64_t      sectors;
} STATE;

void    state_create(STATE *ret);
void    state_destroy(STATE *ret);
void    state_stack_push(STATE *ret, int64_t value);
int64_t state_stack_pop(STATE *ret);

void simulate_machine(const TOKENS *input, STATE *state);

#endif
