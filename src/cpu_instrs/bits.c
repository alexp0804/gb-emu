#include "common.h"
#include "cpu.h"
#include "cpu_impl.h"

void and_a(u8 val) {
    cpu.reg.a &= val;
    FLAG_WRITE(FLAG_Z, cpu.reg.a == 0);
    FLAG_CLEAR(FLAG_N | FLAG_C);
    FLAG_SET(FLAG_H);
}
void and_a_r8() {
    and_a(read_r8_operand(Z_OPERAND(cpu.opcode)));
}
void and_a_n8() {
    and_a(fetch_byte());
}

void or_a(u8 val) {
    cpu.reg.a |= val;
    FLAG_WRITE(FLAG_Z, cpu.reg.a == 0);
    FLAG_CLEAR(FLAG_N | FLAG_H | FLAG_C);
}
void or_a_r8() {
    or_a(read_r8_operand(Z_OPERAND(cpu.opcode)));
}
void or_a_n8() {
    or_a(fetch_byte());
}

void xor_a(u8 val) {
    cpu.reg.a ^= val;
    FLAG_WRITE(FLAG_Z, cpu.reg.a == 0);
    FLAG_CLEAR(FLAG_N | FLAG_H | FLAG_C);
}
void xor_a_r8() {
    xor_a(read_r8_operand(Z_OPERAND(cpu.opcode)));
}
void xor_a_n8() {
    xor_a(fetch_byte());
}

void cpl_a() {
    cpu.reg.a = ~cpu.reg.a;
    FLAG_SET(FLAG_N | FLAG_H);
}

void rl_r8() {
    u8 r8_code = Z_OPERAND(cpu.opcode);
    u8 r8 = read_r8_operand(r8_code);
    u8 bit7 = r8 >> 7;
    u8 new_bit0 = FLAG_IS_SET(FLAG_C);
    u8 result = (r8 << 1) | new_bit0;
    FLAG_WRITE(FLAG_C, bit7);
    FLAG_WRITE(FLAG_Z, result == 0);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    write_r8_operand(r8_code, result);
}
void rla() {
    rl_r8();
    FLAG_CLEAR(FLAG_Z);
}

void rlc_r8() {
    u8 r8_code = Z_OPERAND(cpu.opcode);
    u8 r8 = read_r8_operand(r8_code);
    u8 bit7 = r8 >> 7;
    u8 result = (r8 << 1) | bit7;
    FLAG_WRITE(FLAG_C, bit7);
    FLAG_WRITE(FLAG_Z, result == 0);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    write_r8_operand(r8_code, result);
}
void rlca() {
    rlc_r8();
    FLAG_CLEAR(FLAG_Z);
}

void rr_r8() {
    u8 r8_code = Z_OPERAND(cpu.opcode);
    u8 r8 = read_r8_operand(r8_code);
    u8 bit0 = r8 & 1;
    u8 new_bit7 = FLAG_IS_SET(FLAG_C);
    u8 result = (r8 >> 1) | (new_bit7 << 7);
    FLAG_WRITE(FLAG_C, bit0);
    FLAG_WRITE(FLAG_Z, result == 0);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    write_r8_operand(r8_code, result);
}
void rra() {
    rr_r8();
    FLAG_CLEAR(FLAG_Z);
}

void rrc_r8() {
    u8 r8_code = Z_OPERAND(cpu.opcode);
    u8 r8 = read_r8_operand(r8_code);
    u8 bit0 = r8 & 1;
    u8 result = (r8 >> 1) | (bit0 << 7);
    FLAG_WRITE(FLAG_C, bit0);
    FLAG_WRITE(FLAG_Z, result == 0);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    write_r8_operand(r8_code, result);
}
void rrca() {
    rrc_r8();
    FLAG_CLEAR(FLAG_Z);
}

void sla_r8() {
    u8 r8_code = Z_OPERAND(cpu.opcode);
    u8 r8 = read_r8_operand(r8_code);
    u8 result = r8 << 1;
    FLAG_WRITE(FLAG_Z, result == 0);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    FLAG_WRITE(FLAG_C, r8 >> 7);
    write_r8_operand(r8_code, result);
}

void sra_r8() {
    u8 r8_code = Z_OPERAND(cpu.opcode);
    u8 r8 = read_r8_operand(r8_code);
    u8 result = (r8 & 0x80) | (r8 >> 1);
    FLAG_WRITE(FLAG_Z, result == 0);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    FLAG_WRITE(FLAG_C, r8 & 1);
    write_r8_operand(r8_code, result);
}

void srl_r8() {
    u8 r8_code = Z_OPERAND(cpu.opcode);
    u8 r8 = read_r8_operand(r8_code);
    u8 result = r8 >> 1;
    FLAG_WRITE(FLAG_Z, result == 0);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    FLAG_WRITE(FLAG_C, r8 & 1);
    write_r8_operand(r8_code, result);
}

void swap_r8() {
    u8 r8_code = Z_OPERAND(cpu.opcode);
    u8 r8 = read_r8_operand(r8_code);
    u8 result = (r8 << 4) | (r8 >> 4);
    FLAG_WRITE(FLAG_Z, result == 0);
    FLAG_CLEAR(FLAG_N | FLAG_H | FLAG_C);
    write_r8_operand(r8_code, result);
}

void bit_b3_r8() {
    u8 b3_code = Y_OPERAND(cpu.opcode);
    u8 r8_code = Z_OPERAND(cpu.opcode);
    FLAG_WRITE(FLAG_Z, (read_r8_operand(r8_code) & (1 << b3_code)) == 0);
    FLAG_CLEAR(FLAG_N);
    FLAG_SET(FLAG_H);
}

void res_b3_r8() {
    u8 b3_code = Y_OPERAND(cpu.opcode);
    u8 r8_code = Z_OPERAND(cpu.opcode);
    write_r8_operand(r8_code, read_r8_operand(r8_code) & (~(1 << b3_code)));
}

void set_b3_r8() {
    u8 b3_code = Y_OPERAND(cpu.opcode);
    u8 r8_code = Z_OPERAND(cpu.opcode);
    write_r8_operand(r8_code, read_r8_operand(r8_code) | (1 << b3_code));
}