#include "common.h"
#include "cpu.h"
#include "cpu_impl.h"

void pop_r16stk() {
    write_r16stk_operand(P_OPERAND(cpu.opcode), pop_word());
}
void push_r16stk() {
    push_word(read_r16stk_operand(P_OPERAND(cpu.opcode)));
}