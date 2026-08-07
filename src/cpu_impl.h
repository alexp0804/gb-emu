#pragma once

#include "common.h"
#include "cpu.h"

typedef struct instruction {
    const char* mnemonic;
    u8 length, cycles;
    void (*execute)(void);
} instruction_s;

extern const instruction_s instruction_table[256];
extern const instruction_s bit_shift_instruction_table[8];
extern const instruction_s bit_flag_instruction_table[4];

instruction_s decode_opcode(void);

void unimplemented(void);
void noop(void);