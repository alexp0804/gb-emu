#include "common.h"
#include "cpu.h"
#include "cpu_impl.h"

void dec_r16() {
    u8 r16_code = P_OPERAND(cpu.opcode);
    write_r16_operand(r16_code, read_r16_operand(r16_code) - 1);
}
void inc_r16() {
    u8 r16_code = P_OPERAND(cpu.opcode);
    write_r16_operand(r16_code, read_r16_operand(r16_code) + 1);
}