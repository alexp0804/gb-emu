#pragma once

#include "common.h"
#include "cpu.h"

#define FLAG_Z (1 << 7)
#define FLAG_N (1 << 6)
#define FLAG_H (1 << 5)
#define FLAG_C (1 << 4)

#define FLAG_IS_SET(x) ((cpu.reg.f & (x)) != 0)
#define FLAG_SET(x) (cpu.reg.f |= (x))
#define FLAG_CLEAR(x) (cpu.reg.f &= ~(x))
#define FLAG_WRITE(flag, cond) \
    do {                       \
        if (cond)              \
            FLAG_SET(flag);    \
        else                   \
            FLAG_CLEAR(flag);  \
    } while (0)

// Clears all bits outside the range (hi..lo]
//  e.g. MASK_RANGE(0xff, 0, 8) -> 0xff
//       MASK_RANGE(0xff, 0, 4) -> 0x0f
//       MASK_RANGE(0xff, 1, 3) -> 0x06
#define MASK_RANGE(value, lo, hi) ((((1 << hi) - 1) ^ ((1 << lo) - 1)) & value)

// 0bxx000000
#define X_OPERAND(byte) (MASK_RANGE(byte, 6, 8) >> 6)
// 0b00yyy000
#define Y_OPERAND(byte) (MASK_RANGE(byte, 3, 6) >> 3)
// 0b00000zzz
#define Z_OPERAND(byte) (MASK_RANGE(byte, 0, 3) >> 0)
// 0b00pp0000
#define P_OPERAND(byte) (MASK_RANGE(byte, 4, 6) >> 4)
// 0b000rr000
#define R_OPERAND(byte) (MASK_RANGE(byte, 3, 5) >> 3)

typedef struct instruction {
    const char* mnemonic;
    u8 length, cycles;
    void (*execute)(void);
} instruction_s;

extern const instruction_s instruction_table[256];
extern const instruction_s bit_shift_instruction_table[8];
extern const instruction_s bit_flag_instruction_table[4];

u8 fetch_byte(void);
u16 fetch_word(void);

void write_r8_operand(u8 code, u8 val);
u8 read_r8_operand(u8 code);

void write_r16_operand(u8 code, u16 val);
u16 read_r16_operand(u8 code);

u16 read_r16mem_operand(u8 code);

void write_r16stk_operand(u8 code, u16 val);
u16 read_r16stk_operand(u8 code);

bool condition_met(u8 cond);

instruction_s decode_opcode(void);

void unimplemented(void);
void noop(void);

void ld_r8_r8(void);
void ld_r8_n8(void);
void ld_r16_n16(void);
void ld_at_r16mem_a(void);