#include "common.h"
#include "cpu.h"
#include "cpu_impl.h"

void and_a(u8 val) {
    cpu.reg.a &= val;
    FLAG_WRITE(FLAG_Z, cpu.reg.a == 0);
    FLAG_CLEAR(FLAG_N | FLAG_C);
    FLAG_SET(FLAG_H);
}
void and_a_r8() { and_a(read_r8_operand(Z_OPERAND(cpu.opcode))); }
void and_a_n8() { and_a(fetch_byte()); }

void or_a(u8 val) {
    cpu.reg.a |= val;
    FLAG_WRITE(FLAG_Z, cpu.reg.a == 0);
    FLAG_CLEAR(FLAG_N | FLAG_H | FLAG_C);
}
void or_a_r8() { or_a(read_r8_operand(Z_OPERAND(cpu.opcode))); }
void or_a_n8() { or_a(fetch_byte()); }

void xor_a(u8 val) {
    cpu.reg.a ^= val;
    FLAG_WRITE(FLAG_Z, cpu.reg.a == 0);
    FLAG_CLEAR(FLAG_N | FLAG_H | FLAG_C);
}
void xor_a_r8() { xor_a(read_r8_operand(Z_OPERAND(cpu.opcode))); }
void xor_a_n8() { xor_a(fetch_byte()); }

void cpl_a() {
    cpu.reg.a = ~cpu.reg.a;
    FLAG_SET(FLAG_N | FLAG_H);
}