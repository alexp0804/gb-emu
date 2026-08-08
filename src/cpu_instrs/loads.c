#include "cpu.h"
#include "cpu_impl.h"
#include "mem.h"

void ld_r8_r8() { write_r8_operand(Y_OPERAND(cpu.opcode), read_r8_operand(Z_OPERAND(cpu.opcode))); }
void ld_r8_n8() { write_r8_operand(Y_OPERAND(cpu.opcode), fetch_byte()); }

void ld_r16_n16() { write_r16_operand(P_OPERAND(cpu.opcode), fetch_word()); }

void ld_at_r16mem_a() { write_mem(read_r16mem_operand(P_OPERAND(cpu.opcode)), cpu.reg.a); }
void ld_a_at_r16mem() { cpu.reg.a = read_mem(P_OPERAND(cpu.opcode)); }

void ld_at_n16_a() { write_mem(fetch_word(), cpu.reg.a); }
void ld_a_at_n16() { cpu.reg.a = read_mem(fetch_word()); }

void ldh_at_n8_a() { write_mem(IO_START + fetch_byte(), cpu.reg.a); }
void ldh_a_at_n8() { cpu.reg.a = read_mem(IO_START + fetch_byte()); }

void ldh_at_c_a() { write_mem(IO_START + cpu.reg.c, cpu.reg.a); }
void ldh_a_at_c() { cpu.reg.a = read_mem(IO_START + cpu.reg.c); }
