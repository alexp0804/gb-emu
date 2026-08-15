#include "common.h"
#include "cpu.h"
#include "cpu_impl.h"

void jp_hl() {
    cpu.reg.pc = cpu.reg.hl;
}
void jp_n16() {
    cpu.reg.pc = fetch_word();
}
void jp_cond_n16() {
    u16 n16 = fetch_word();
    if (condition_met(R_OPERAND(cpu.opcode))) {
        cpu.reg.pc = n16;
        cpu.extra_cycles = 1;
    }
}

void jr_d8() {
    cpu.reg.pc += (i8)fetch_byte();
}
void jr_cond_d8() {
    i8 d8 = (i8)fetch_byte();
    if (condition_met(R_OPERAND(cpu.opcode))) {
        cpu.reg.pc += d8;
        cpu.extra_cycles = 1;
    }
}

void call_n16() {
    u16 d16 = fetch_word();
    push_word(cpu.reg.pc);
    cpu.reg.pc = d16;
}
void call_cond_n16() {
    u16 d16 = fetch_word();
    if (condition_met(R_OPERAND(cpu.opcode))) {
        push_word(cpu.reg.pc);
        cpu.reg.pc = d16;
        cpu.extra_cycles = 3;
    }
}

void ret() {
    cpu.reg.pc = pop_word();
}
void ret_cond() {
    if (condition_met(R_OPERAND(cpu.opcode))) {
        ret();
        cpu.extra_cycles = 3;
    }
}
void reti() {
    ei();
    ret();
}

void rst_tgt3() {
    push_word(cpu.reg.pc);
    cpu.reg.pc = 8 * Y_OPERAND(cpu.opcode);
}