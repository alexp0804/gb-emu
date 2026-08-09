#include "common.h"
#include "cpu.h"
#include "cpu_impl.h"

u8 add(u8 a, u8 b) {
    u8 result = a + b;
    FLAG_WRITE(FLAG_Z, result == 0);
    FLAG_CLEAR(FLAG_N);
    FLAG_WRITE(FLAG_H, (a & 0x0F) + (b & 0x0F) >= 0x10);
    FLAG_WRITE(FLAG_C, result < a);
    return result;
}
void add_a_r8() { cpu.reg.a = add(cpu.reg.a, read_r8_operand(Z_OPERAND(cpu.opcode))); }
void add_a_n8() { cpu.reg.a = add(cpu.reg.a, fetch_byte()); }

u8 adc(u8 a, u8 b) { return add(a, b + FLAG_IS_SET(FLAG_C)); }
void adc_a_r8() { cpu.reg.a = adc(cpu.reg.a, read_r8_operand(Z_OPERAND(cpu.opcode))); }
void adc_a_n8() { cpu.reg.a = adc(cpu.reg.a, fetch_byte()); }

u8 sub(u8 a, u8 b) {
    u8 result = a - b;
    FLAG_WRITE(FLAG_Z, result == 0);
    FLAG_SET(FLAG_N);
    FLAG_WRITE(FLAG_H, (a & 0x0F) < (b & 0x0F));
    FLAG_WRITE(FLAG_C, result > a);
    return result;
}
void sub_a_r8() { cpu.reg.a = sub(cpu.reg.a, read_r8_operand(Z_OPERAND(cpu.opcode))); }
void sub_a_n8() { cpu.reg.a = sub(cpu.reg.a, fetch_byte()); }

u8 sbc(u8 a, u8 b) { return sub(a, b + FLAG_IS_SET(FLAG_C)); }
void sbc_a_r8() { cpu.reg.a = sbc(cpu.reg.a, read_r8_operand(Z_OPERAND(cpu.opcode))); }
void sbc_a_n8() { cpu.reg.a = sbc(cpu.reg.a, fetch_byte()); }

void cp(u8 a, u8 b) { u8 result = sub(a, b); }
void cp_a_r8() { cp(cpu.reg.a, read_r8_operand(Z_OPERAND(cpu.opcode))); }
void cp_a_n8() { cp(cpu.reg.a, fetch_byte()); }

void inc_r8() {
    u8 r8_code = Y_OPERAND(cpu.opcode);
    u8 c = FLAG_IS_SET(FLAG_C);
    u8 result = add(read_r8_operand(r8_code), 1);
    FLAG_WRITE(FLAG_C, c);  // This instr shouldn't modify C flag, but add() does.
    write_r8_operand(r8_code, result);
}
void dec_r8() {
    u8 r8_code = Y_OPERAND(cpu.opcode);
    u8 c = FLAG_IS_SET(FLAG_C);
    u8 result = sub(read_r8_operand(r8_code), 1);
    FLAG_WRITE(FLAG_C, c);  // This instr shouldn't modify FLAG_C, but sub() does.
    write_r8_operand(r8_code, result);
}

void inc_r16() {
    u8 r16_code = P_OPERAND(cpu.opcode);
    write_r16_operand(r16_code, read_r16_operand(r16_code) + 1);
}
void dec_r16() {
    u8 r16_code = P_OPERAND(cpu.opcode);
    write_r16_operand(r16_code, read_r16_operand(r16_code) - 1);
}

void add_hl_r16() {
    u16 r16 = read_r16_operand(Y_OPERAND(cpu.opcode));
    u16 result = cpu.reg.hl + r16;
    FLAG_CLEAR(FLAG_N);
    FLAG_WRITE(FLAG_H, (cpu.reg.hl & 0x0FFF) + (r16 & 0x0FFF) >= 0x1000);
    FLAG_WRITE(FLAG_C, result < cpu.reg.hl);
    cpu.reg.hl = result;
}