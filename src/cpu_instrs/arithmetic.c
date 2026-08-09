#include "common.h"
#include "cpu.h"
#include "cpu_impl.h"

void dec_r8() {
    u8 r8_code = Y_OPERAND(cpu.opcode);
    u8 r8 = read_r8_operand(r8_code);
    u8 result = r8 - 1;
    FLAG_WRITE(FLAG_Z, result == 0);
    FLAG_SET(FLAG_N);
    FLAG_WRITE(FLAG_H, (r8 & 0x0F) == 0);
    write_r8_operand(r8_code, result);
}
void inc_r8() {
    u8 r8_code = Y_OPERAND(cpu.opcode);
    u8 r8 = read_r8_operand(r8_code);
    u8 result = r8 + 1;
    FLAG_WRITE(FLAG_Z, result == 0);
    FLAG_SET(FLAG_N);
    FLAG_WRITE(FLAG_H, (r8 & 0x0F) + 1 >= 0x10);
    write_r8_operand(r8_code, result);
}

void dec_r16() {
    u8 r16_code = P_OPERAND(cpu.opcode);
    write_r16_operand(r16_code, read_r16_operand(r16_code) - 1);
}
void inc_r16() {
    u8 r16_code = P_OPERAND(cpu.opcode);
    write_r16_operand(r16_code, read_r16_operand(r16_code) + 1);
}