#include "cpu.h"
#include "cpu_impl.h"

void ld_r8_r8() { write_r8_operand(Y_OPERAND(cpu.opcode), read_r8_operand(Z_OPERAND(cpu.opcode))); }
void ld_r8_n8() { write_r8_operand(Y_OPERAND(cpu.opcode), fetch_byte()); }