#include "common.h"
#include "cpu.h"
#include "cpu_impl.h"

void jp_hl() { cpu.reg.pc = cpu.reg.hl; }
void jp_n16() { cpu.reg.pc = fetch_word(); }
void jp_cond_n16() {
    u16 n16 = fetch_word();
    if (condition_met(R_OPERAND(cpu.opcode))) cpu.reg.pc = n16;
}

void jr_d8() { cpu.reg.pc += (i8)fetch_byte(); }
void jr_cond_d8() {
    i8 d8 = (i8)fetch_byte();
    if (condition_met(R_OPERAND(cpu.opcode))) cpu.reg.pc += d8;
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
    }
}

void ret() { cpu.reg.pc = pop_word(); }
void ret_cond() {
    if (condition_met(R_OPERAND(cpu.opcode))) ret();
}
void reti() {
    ei();
    ret();
}