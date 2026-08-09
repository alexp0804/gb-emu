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
extern const instruction_s cb_instruction_table[256];

u8 fetch_byte(void);
u16 fetch_word(void);

void push_word(u16 val);
u16 pop_word();

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
void invalid(void);

void noop(void);

void ld_r8_r8(void);
void ld_r8_n8(void);
void ld_r16_n16(void);
void ld_at_r16mem_a(void);
void ld_a_at_r16mem(void);
void ld_at_n16_a(void);
void ld_a_at_n16(void);
void ldh_at_c_a(void);
void ldh_a_at_c(void);
void ldh_at_n8_a(void);
void ldh_a_at_n8(void);
void ld_sp_hl(void);

void and_a_r8(void);
void and_a_n8(void);
void or_a_r8(void);
void or_a_n8(void);
void xor_a_r8(void);
void xor_a_n8(void);
void cpl_a(void);

void dec_r8(void);
void inc_r8(void);
void dec_r16(void);
void inc_r16(void);

void jp_hl(void);
void jp_n16(void);
void jp_cond_n16(void);
void jr_d8(void);
void jr_cond_d8(void);
void call_n16(void);
void call_cond_n16(void);
void ret(void);
void ret_cond(void);

void rl_r8(void);
void rlc_r8(void);
void rr_r8(void);
void rrc_r8(void);
void sla_r8(void);
void sra_r8(void);
void srl_r8(void);
void swap_r8(void);
void bit_b3_r8(void);
void res_b3_r8(void);
void set_b3_r8(void);

void pop_r16stk(void);
void push_r16stk(void);