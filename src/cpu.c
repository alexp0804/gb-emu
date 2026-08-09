#include "cpu.h"

#include "cpu_impl.h"
#include "mem.h"

cpu_s cpu;

// Todo: in the case of branching instructions, cycle lengths may be different based on if the
// condition for that instruction is met or not.

u8 fetch_byte(void) { return read_mem(cpu.reg.pc++); }
u16 fetch_word(void) {
    u8 lo = fetch_byte();
    u8 hi = fetch_byte();
    return (hi << 8) | lo;
}

void push_word(u16 val) {
    write_mem(--cpu.reg.sp, (val & 0xFF00) >> 8);
    write_mem(--cpu.reg.sp, (val & 0x00FF));
}
u16 pop_word() {
    u16 lo = read_mem(cpu.reg.sp++);
    u16 hi = read_mem(cpu.reg.sp++);

    return (hi << 8) | lo;
}

void write_r8_operand(u8 code, u8 val) {
    switch (code) {
        case 0:
            cpu.reg.b = val;
            return;
        case 1:
            cpu.reg.c = val;
            return;
        case 2:
            cpu.reg.d = val;
            return;
        case 3:
            cpu.reg.e = val;
            return;
        case 4:
            cpu.reg.h = val;
            return;
        case 5:
            cpu.reg.l = val;
            return;
        case 6:
            write_mem(cpu.reg.hl, val);
            return;
        case 7:
            cpu.reg.a = val;
            return;
    }
    printf("Invalid write to r8 operand.\n");
    exit(1);
}
u8 read_r8_operand(u8 code) {
    switch (code) {
        case 0:
            return cpu.reg.b;
        case 1:
            return cpu.reg.c;
        case 2:
            return cpu.reg.d;
        case 3:
            return cpu.reg.e;
        case 4:
            return cpu.reg.h;
        case 5:
            return cpu.reg.l;
        case 6:
            return read_mem(cpu.reg.hl);
        case 7:
            return cpu.reg.a;
    }
    printf("Invalid read of r8 operand.\n");
    exit(1);
}

void write_r16_operand(u8 code, u16 val) {
    switch (code) {
        case 0:
            cpu.reg.bc = val;
            return;
        case 1:
            cpu.reg.de = val;
            return;
        case 2:
            cpu.reg.hl = val;
            return;
        case 3:
            cpu.reg.sp = val;
            return;
    }
    printf("Invalid write to r16 operand.\n");
    exit(1);
}
u16 read_r16_operand(u8 code) {
    switch (code) {
        case 0:
            return cpu.reg.bc;
        case 1:
            return cpu.reg.de;
        case 2:
            return cpu.reg.hl;
        case 3:
            return cpu.reg.sp;
    }
    printf("Invalid read of r16 operand.\n");
    exit(1);
}

u16 read_r16mem_operand(u8 code) {
    switch (code) {
        case 0:
            return cpu.reg.bc;
        case 1:
            return cpu.reg.de;
        case 2:
            return cpu.reg.hl++;
        case 3:
            return cpu.reg.hl--;
    }
    printf("Invalid read of r16mem operand.\n");
    exit(1);
}

void write_r16stk_operand(u8 code, u16 val) {
    switch (code) {
        case 0:
            cpu.reg.bc = val;
            return;
        case 1:
            cpu.reg.de = val;
            return;
        case 2:
            cpu.reg.hl = val;
            return;
        case 3:
            cpu.reg.af = val & 0xFFF0;
            return;
    }
    printf("Invalid write to r16stk operand.\n");
    exit(1);
}
u16 read_r16stk_operand(u8 code) {
    switch (code) {
        case 0:
            return cpu.reg.bc;
        case 1:
            return cpu.reg.de;
        case 2:
            return cpu.reg.hl;
        case 3:
            return cpu.reg.af;
    }
    printf("Invalid read of r16stk operand.\n");
    exit(1);
}

bool condition_met(u8 cond) {
    switch (cond) {
        case 0:
            return !FLAG_IS_SET(FLAG_Z);
        case 1:
            return FLAG_IS_SET(FLAG_Z);
        case 2:
            return !FLAG_IS_SET(FLAG_C);
        case 3:
            return FLAG_IS_SET(FLAG_C);
    }
    printf("Attempted to decode an invalid condition code.\n");
    exit(1);
}

// clang-format off
const instruction_s instruction_table[256] = {
    [0x00] = {"nop", 0, 0, noop},       
    [0x01] = {"LD BC, n16", 3, 3, ld_r16_n16},
    [0x02] = {"LD (BC), A", 1, 2, ld_at_r16mem_a},
    [0x03] = {"INC BC", 1, 2, inc_r16},
    [0x04] = {"INC B", 1, 1, inc_r8},
    [0x05] = {"DEC B", 1, 1, dec_r8},
    [0x06] = {"LD B, n8", 2, 2, ld_r8_n8},
    [0x07] = {"", 0, 0, unimplemented},
    [0x08] = {"", 0, 0, unimplemented},
    [0x09] = {"", 0, 0, unimplemented},
    [0x0a] = {"LD A, (BC)", 1, 2, ld_a_at_r16mem},
    [0x0b] = {"DEC BC", 1, 2, dec_r16},
    [0x0c] = {"INC C", 1, 1, inc_r8},
    [0x0d] = {"DEC C", 1, 1, dec_r8},
    [0x0e] = {"LD C, n8", 2, 2, ld_r8_n8},
    [0x0f] = {"", 0, 0, unimplemented},
    [0x10] = {"", 0, 0, unimplemented},
    [0x11] = {"LD DE, n16", 3, 3, ld_r16_n16},
    [0x12] = {"LD (DE), A", 1, 2, ld_at_r16mem_a},
    [0x13] = {"INC DE", 1, 2, inc_r16},
    [0x14] = {"INC D", 1, 1, inc_r8},
    [0x15] = {"DEC D", 1, 1, dec_r8},
    [0x16] = {"LD D, n8", 2, 2, ld_r8_n8},
    [0x17] = {"", 0, 0, unimplemented},
    [0x18] = {"JR d8", 2, 3, jr_d8},
    [0x19] = {"", 0, 0, unimplemented},
    [0x1a] = {"LD A, (DE)", 1, 2, ld_a_at_r16mem},
    [0x1b] = {"DEC DE", 1, 2, dec_r16},
    [0x1c] = {"INC E", 1, 1, inc_r8},
    [0x1d] = {"DEC E", 1, 1, dec_r8},
    [0x1e] = {"LD E, n8", 2, 2, ld_r8_n8},
    [0x1f] = {"", 0, 0, unimplemented},
    [0x20] = {"JR NZ, d8", 2, 2, jr_cond_d8},
    [0x21] = {"LD HL, n16", 3, 3, ld_r16_n16},
    [0x22] = {"LD (HL+), A", 1, 2, ld_at_r16mem_a},
    [0x23] = {"INC HL", 1, 2, inc_r16},
    [0x24] = {"INC H", 1, 1, inc_r8},
    [0x25] = {"DEC H", 1, 1, dec_r8},
    [0x26] = {"LD H, n8", 2, 2, ld_r8_n8},
    [0x27] = {"", 0, 0, unimplemented},
    [0x28] = {"JR Z, d8", 2, 2, jr_cond_d8},
    [0x29] = {"", 0, 0, unimplemented},
    [0x2a] = {"LD A, (HL+)", 1, 2, ld_a_at_r16mem},
    [0x2b] = {"DEC HL", 1, 2, dec_r16},
    [0x2c] = {"INC L", 1, 1, inc_r8},
    [0x2d] = {"DEC L", 1, 1, dec_r8},
    [0x2e] = {"LD L, n8", 2, 2, ld_r8_n8},
    [0x2f] = {"CPL", 1, 1, cpl_a},
    [0x30] = {"JR NC, d8", 2, 2, jr_cond_d8},
    [0x31] = {"LD SP, n16", 3, 3, ld_r16_n16},
    [0x32] = {"LD (HL-), A", 1, 2, ld_at_r16mem_a},
    [0x33] = {"INC SP", 1, 2, inc_r16},
    [0x34] = {"INC (HL)", 1, 3, inc_r8},
    [0x35] = {"DEC (HL)", 1, 3, dec_r8},
    [0x36] = {"LD (HL), n8", 2, 2, ld_r8_n8},
    [0x37] = {"SCF", 1, 1, scf},
    [0x38] = {"JR C, d8", 2, 2, jr_cond_d8},
    [0x39] = {"", 0, 0, unimplemented},
    [0x3a] = {"LD A (HL-)", 1, 2, ld_a_at_r16mem},
    [0x3b] = {"DEC SP", 1, 2, dec_r16},
    [0x3c] = {"INC A", 1, 1, inc_r8},
    [0x3d] = {"DEC A", 1, 1, dec_r8},
    [0x3e] = {"LD A, n8", 2, 2, ld_r8_n8},
    [0x3f] = {"CCF", 1, 1, ccf},
    [0x40] = {"LD B, B",    1, 1, ld_r8_r8},
    [0x41] = {"LD B, C",    1, 1, ld_r8_r8},
    [0x42] = {"LD B, D",    1, 1, ld_r8_r8},
    [0x43] = {"LD B, E",    1, 1, ld_r8_r8},
    [0x44] = {"LD B, H",    1, 1, ld_r8_r8},
    [0x45] = {"LD B, L",    1, 1, ld_r8_r8},
    [0x46] = {"LD B, (HL)", 1, 2, ld_r8_r8},
    [0x47] = {"LD B, A",    1, 1, ld_r8_r8},
    [0x48] = {"LD C, B",    1, 1, ld_r8_r8},
    [0x49] = {"LD C, C",    1, 1, ld_r8_r8},
    [0x4a] = {"LD C, D",    1, 1, ld_r8_r8},
    [0x4b] = {"LD C, E",    1, 1, ld_r8_r8},
    [0x4c] = {"LD C, H",    1, 1, ld_r8_r8},
    [0x4d] = {"LD C, L",    1, 1, ld_r8_r8},
    [0x4e] = {"LD C, (HL)", 1, 2, ld_r8_r8},
    [0x4f] = {"LD C, A",    1, 1, ld_r8_r8},
    [0x50] = {"LD D, B",    1, 1, ld_r8_r8},
    [0x51] = {"LD D, C",    1, 1, ld_r8_r8},
    [0x52] = {"LD D, D",    1, 1, ld_r8_r8},
    [0x53] = {"LD D, E",    1, 1, ld_r8_r8},
    [0x54] = {"LD D, H",    1, 1, ld_r8_r8},
    [0x55] = {"LD D, L",    1, 1, ld_r8_r8},
    [0x56] = {"LD D, (HL)", 1, 2, ld_r8_r8},
    [0x57] = {"LD D, A",    1, 1, ld_r8_r8},
    [0x58] = {"LD E, B",    1, 1, ld_r8_r8},
    [0x59] = {"LD E, C",    1, 1, ld_r8_r8},
    [0x5a] = {"LD E, D",    1, 1, ld_r8_r8},
    [0x5b] = {"LD E, E",    1, 1, ld_r8_r8},
    [0x5c] = {"LD E, H",    1, 1, ld_r8_r8},
    [0x5d] = {"LD E, L",    1, 1, ld_r8_r8},
    [0x5e] = {"LD E, (HL)", 1, 2, ld_r8_r8},
    [0x5f] = {"LD E, A",    1, 1, ld_r8_r8},
    [0x60] = {"LD H, B",    1, 1, ld_r8_r8},
    [0x61] = {"LD H, C",    1, 1, ld_r8_r8},
    [0x62] = {"LD H, D",    1, 1, ld_r8_r8},
    [0x63] = {"LD H, E",    1, 1, ld_r8_r8},
    [0x64] = {"LD H, H",    1, 1, ld_r8_r8},
    [0x65] = {"LD H, L",    1, 1, ld_r8_r8},
    [0x66] = {"LD H, (HL)", 1, 2, ld_r8_r8},
    [0x67] = {"LD H, A",    1, 1, ld_r8_r8},
    [0x68] = {"LD L, B",    1, 1, ld_r8_r8},
    [0x69] = {"LD L, C",    1, 1, ld_r8_r8},
    [0x6a] = {"LD L, D",    1, 1, ld_r8_r8},
    [0x6b] = {"LD L, E",    1, 1, ld_r8_r8},
    [0x6c] = {"LD L, H",    1, 1, ld_r8_r8},
    [0x6d] = {"LD L, L",    1, 1, ld_r8_r8},
    [0x6e] = {"LD L, (HL)", 1, 2, ld_r8_r8},
    [0x6f] = {"LD L, A",    1, 1, ld_r8_r8},
    [0x70] = {"LD (HL), B", 1, 2, ld_r8_r8},
    [0x71] = {"LD (HL), C", 1, 2, ld_r8_r8},
    [0x72] = {"LD (HL), D", 1, 2, ld_r8_r8},
    [0x73] = {"LD (HL), E", 1, 2, ld_r8_r8},
    [0x74] = {"LD (HL), H", 1, 2, ld_r8_r8},
    [0x75] = {"LD (HL), L", 1, 2, ld_r8_r8},
    [0x76] = {"halt", 0, 0, unimplemented},
    [0x77] = {"LD (HL), A", 1, 2, ld_r8_r8},
    [0x78] = {"LD A, B",    1, 1, ld_r8_r8},
    [0x79] = {"LD A, C",    1, 1, ld_r8_r8},
    [0x7a] = {"LD A, D",    1, 1, ld_r8_r8},
    [0x7b] = {"LD A, E",    1, 1, ld_r8_r8},
    [0x7c] = {"LD A, H",    1, 1, ld_r8_r8},
    [0x7d] = {"LD A, L",    1, 1, ld_r8_r8},
    [0x7e] = {"LD A, (HL)", 1, 2, ld_r8_r8},
    [0x7f] = {"LD A, A",    1, 1, ld_r8_r8},
    [0x80] = {"", 0, 0, unimplemented},
    [0x81] = {"", 0, 0, unimplemented},
    [0x82] = {"", 0, 0, unimplemented},
    [0x83] = {"", 0, 0, unimplemented},
    [0x84] = {"", 0, 0, unimplemented},
    [0x85] = {"", 0, 0, unimplemented},
    [0x86] = {"", 0, 0, unimplemented},
    [0x87] = {"", 0, 0, unimplemented},
    [0x88] = {"", 0, 0, unimplemented},
    [0x89] = {"", 0, 0, unimplemented},
    [0x8a] = {"", 0, 0, unimplemented},
    [0x8b] = {"", 0, 0, unimplemented},
    [0x8c] = {"", 0, 0, unimplemented},
    [0x8d] = {"", 0, 0, unimplemented},
    [0x8e] = {"", 0, 0, unimplemented},
    [0x8f] = {"", 0, 0, unimplemented},
    [0x90] = {"", 0, 0, unimplemented},
    [0x91] = {"", 0, 0, unimplemented},
    [0x92] = {"", 0, 0, unimplemented},
    [0x93] = {"", 0, 0, unimplemented},
    [0x94] = {"", 0, 0, unimplemented},
    [0x95] = {"", 0, 0, unimplemented},
    [0x96] = {"", 0, 0, unimplemented},
    [0x97] = {"", 0, 0, unimplemented},
    [0x98] = {"", 0, 0, unimplemented},
    [0x99] = {"", 0, 0, unimplemented},
    [0x9a] = {"", 0, 0, unimplemented},
    [0x9b] = {"", 0, 0, unimplemented},
    [0x9c] = {"", 0, 0, unimplemented},
    [0x9d] = {"", 0, 0, unimplemented},
    [0x9e] = {"", 0, 0, unimplemented},
    [0x9f] = {"", 0, 0, unimplemented},
    [0xa0] = {"AND B", 1, 1, and_a_r8 },
    [0xa1] = {"AND C", 1, 1, and_a_r8},
    [0xa2] = {"AND D", 1, 1, and_a_r8},
    [0xa3] = {"AND E", 1, 1, and_a_r8},
    [0xa4] = {"AND H", 1, 1, and_a_r8},
    [0xa5] = {"AND L", 1, 1, and_a_r8},
    [0xa6] = {"AND (HL)", 1, 2, and_a_r8},
    [0xa7] = {"AND A", 1, 1, and_a_r8},
    [0xa8] = {"XOR B", 1, 1, xor_a_r8},
    [0xa9] = {"XOR C", 1, 1, xor_a_r8},
    [0xaa] = {"XOR D", 1, 1, xor_a_r8},
    [0xab] = {"XOR E", 1, 1, xor_a_r8},
    [0xac] = {"XOR H", 1, 1, xor_a_r8},
    [0xad] = {"XOR L", 1, 1, xor_a_r8},
    [0xae] = {"XOR (HL)", 1, 2, xor_a_r8},
    [0xaf] = {"XOR A", 1, 1, xor_a_r8},
    [0xb0] = {"OR B", 1, 1, or_a_r8},
    [0xb1] = {"OR C", 1, 1, or_a_r8},
    [0xb2] = {"OR D", 1, 1, or_a_r8},
    [0xb3] = {"OR E", 1, 1, or_a_r8},
    [0xb4] = {"OR H", 1, 1, or_a_r8},
    [0xb5] = {"OR L", 1, 1, or_a_r8},
    [0xb6] = {"OR (HL)", 1, 2, or_a_r8},
    [0xb7] = {"OR A", 1, 1, or_a_r8},
    [0xb8] = {"", 0, 0, unimplemented},
    [0xb9] = {"", 0, 0, unimplemented},
    [0xba] = {"", 0, 0, unimplemented},
    [0xbb] = {"", 0, 0, unimplemented},
    [0xbc] = {"", 0, 0, unimplemented},
    [0xbd] = {"", 0, 0, unimplemented},
    [0xbe] = {"", 0, 0, unimplemented},
    [0xbf] = {"", 0, 0, unimplemented},
    [0xc0] = {"RET NZ", 1, 2, ret_cond},
    [0xc1] = {"POP BC", 1, 3, pop_r16stk},
    [0xc2] = {"JP NZ, n16", 3, 3, jp_cond_n16},
    [0xc3] = {"JP n16", 3, 4, jp_n16},
    [0xc4] = {"CALL NZ, n16", 3, 3, call_cond_n16},
    [0xc5] = {"PUSH BC", 1, 4, push_r16stk},
    [0xc6] = {"", 0, 0, unimplemented},
    [0xc7] = {"RST 0", 1, 4, rst_tgt3},
    [0xc8] = {"RET Z", 1, 2, ret_cond},
    [0xc9] = {"RET", 1, 4, ret},
    [0xca] = {"JP Z, n16", 3, 4, jp_cond_n16},
    [0xcb] = {"", 0, 0, invalid},
    [0xcc] = {"CALL Z, n16", 3, 3, call_cond_n16},
    [0xcd] = {"CALL n16", 3, 6, call_n16},
    [0xce] = {"", 0, 0, unimplemented},
    [0xcf] = {"RST 1", 1, 4, rst_tgt3},
    [0xd0] = {"RET NC", 1, 2, ret_cond},
    [0xd1] = {"POP DE", 1, 3, pop_r16stk},
    [0xd2] = {"JP NC, n16", 3, 3, jp_cond_n16},
    [0xd3] = {"", 0, 0, invalid},
    [0xd4] = {"CALL NC, n16", 3, 3, call_cond_n16},
    [0xd5] = {"PUSH DE", 1, 4, push_r16stk},
    [0xd6] = {"", 0, 0, unimplemented},
    [0xd7] = {"RST 2", 1, 4, rst_tgt3},
    [0xd8] = {"RET C", 1, 2, ret_cond},
    [0xd9] = {"RETI", 1, 4, reti},
    [0xda] = {"JP C, n16", 3, 3, jp_cond_n16},
    [0xdb] = {"", 0, 0, invalid},
    [0xdc] = {"CALL C, d16", 3, 3, call_cond_n16},
    [0xdd] = {"", 0, 0, invalid},
    [0xde] = {"", 0, 0, unimplemented},
    [0xdf] = {"RST 3", 1, 4, rst_tgt3},
    [0xe0] = {"LDH (n8), A", 2, 3, ldh_at_n8_a},
    [0xe1] = {"POP HL", 1, 3, pop_r16stk},
    [0xe2] = {"LDH (C), A", 1, 2, ldh_at_c_a},
    [0xe3] = {"", 0, 0, invalid},
    [0xe4] = {"", 0, 0, invalid},
    [0xe5] = {"PUSH HL", 1, 4, push_r16stk},
    [0xe6] = {"AND n8", 2, 2, and_a_n8},
    [0xe7] = {"RST 4", 1, 4, rst_tgt3},
    [0xe8] = {"", 0, 0, unimplemented},
    [0xe9] = {"JP HL", 1, 1, jp_hl},
    [0xea] = {"LD (n16), A", 3, 4, ld_at_n16_a},
    [0xeb] = {"", 0, 0, invalid},
    [0xec] = {"", 0, 0, invalid},
    [0xed] = {"", 0, 0, invalid},
    [0xee] = {"XOR n8", 2, 2, xor_a_n8},
    [0xef] = {"RST 5", 1, 4, rst_tgt3},
    [0xf0] = {"LDH A, (n8)", 2, 3, ldh_a_at_n8},
    [0xf1] = {"POP AF", 1, 3, pop_r16stk},
    [0xf2] = {"LDH A, (C)", 1, 2, ldh_a_at_c},
    [0xf3] = {"DI", 1, 1, di},
    [0xf4] = {"", 0, 0, invalid},
    [0xf5] = {"PUSH AF", 1, 4, push_r16stk},
    [0xf6] = {"OR n8", 2, 2, or_a_n8},
    [0xf7] = {"RST 6", 1, 4, rst_tgt3},
    [0xf8] = {"", 0, 0, unimplemented},
    [0xf9] = {"LD SP, HL", 1, 2, ld_sp_hl},
    [0xfa] = {"LD A, (n16)", 3, 4, ld_a_at_n16},
    [0xfb] = {"EI", 1, 1, ei},
    [0xfc] = {"", 0, 0, invalid},
    [0xfd] = {"", 0, 0, invalid},
    [0xfe] = {"", 0, 0, unimplemented},
    [0xff] = {"RST 7", 1, 4, rst_tgt3},
};
const instruction_s cb_instruction_table[256] = {
    [0x00] = {"RLC B", 2, 2, rlc_r8},
    [0x01] = {"RLC C", 2, 2, rlc_r8},
    [0x02] = {"RLC D", 2, 2, rlc_r8},
    [0x03] = {"RLC E", 2, 2, rlc_r8},
    [0x04] = {"RLC H", 2, 2, rlc_r8},
    [0x05] = {"RLC L", 2, 2, rlc_r8},
    [0x06] = {"RLC (HL)", 2, 4, rlc_r8},
    [0x07] = {"RLC A", 2, 2, rlc_r8},
    [0x08] = {"RRC B", 2, 2, rrc_r8},
    [0x09] = {"RRC C", 2, 2, rrc_r8},
    [0x0a] = {"RRC D", 2, 2, rrc_r8},
    [0x0b] = {"RRC E", 2, 2, rrc_r8},
    [0x0c] = {"RRC H", 2, 2, rrc_r8},
    [0x0d] = {"RRC L", 2, 2, rrc_r8},
    [0x0e] = {"RRC (HL)", 2, 4, rrc_r8},
    [0x0f] = {"RRC A", 2, 2, rrc_r8},
    [0x10] = {"RL B", 2, 2, rl_r8},
    [0x11] = {"RL C", 2, 2, rl_r8},
    [0x12] = {"RL D", 2, 2, rl_r8},
    [0x13] = {"RL E", 2, 2, rl_r8},
    [0x14] = {"RL H", 2, 2, rl_r8},
    [0x15] = {"RL L", 2, 2, rl_r8},
    [0x16] = {"RL (HL)", 2, 4, rl_r8},
    [0x17] = {"RL A", 2, 2, rl_r8},
    [0x18] = {"RR B", 2, 2, rr_r8},
    [0x19] = {"RR C", 2, 2, rr_r8},
    [0x1a] = {"RR D", 2, 2, rr_r8},
    [0x1b] = {"RR E", 2, 2, rr_r8},
    [0x1c] = {"RR H", 2, 2, rr_r8},
    [0x1d] = {"RR L", 2, 2, rr_r8},
    [0x1e] = {"RR (HL)", 2, 4, rr_r8},
    [0x1f] = {"RR A", 2, 2, rr_r8},
    [0x20] = {"SLA B", 2, 2, sla_r8},
    [0x21] = {"SLA C", 2, 2, sla_r8},
    [0x22] = {"SLA D", 2, 2, sla_r8},
    [0x23] = {"SLA E", 2, 2, sla_r8},
    [0x24] = {"SLA H", 2, 2, sla_r8},
    [0x25] = {"SLA L", 2, 2, sla_r8},
    [0x26] = {"SLA (HL)", 2, 4, sla_r8},
    [0x27] = {"SLA A", 2, 2, sla_r8},
    [0x28] = {"SRA B", 2, 2, sra_r8},
    [0x29] = {"SRA C", 2, 2, sra_r8},
    [0x2a] = {"SRA D", 2, 2, sra_r8},
    [0x2b] = {"SRA E", 2, 2, sra_r8},
    [0x2c] = {"SRA H", 2, 2, sra_r8},
    [0x2d] = {"SRA L", 2, 2, sra_r8},
    [0x2e] = {"SRA (HL)", 2, 4, sra_r8},
    [0x2f] = {"SRA A", 2, 2, sra_r8},
    [0x30] = {"SWAP B", 2, 2, swap_r8},
    [0x31] = {"SWAP C", 2, 2, swap_r8},
    [0x32] = {"SWAP D", 2, 2, swap_r8},
    [0x33] = {"SWAP E", 2, 2, swap_r8},
    [0x34] = {"SWAP H", 2, 2, swap_r8},
    [0x35] = {"SWAP L", 2, 2, swap_r8},
    [0x36] = {"SWAP (HL)", 2, 4, swap_r8},
    [0x37] = {"SWAP A", 2, 2, swap_r8},
    [0x38] = {"SRL B", 2, 2, srl_r8},
    [0x39] = {"SRL C", 2, 2, srl_r8},
    [0x3a] = {"SRL D", 2, 2, srl_r8},
    [0x3b] = {"SRL E", 2, 2, srl_r8},
    [0x3c] = {"SRL H", 2, 2, srl_r8},
    [0x3d] = {"SRL L", 2, 2, srl_r8},
    [0x3e] = {"SRL (HL)", 2, 4, srl_r8},
    [0x3f] = {"SRL A", 2, 2, srl_r8},
    [0x40] = {"BIT 0 B", 2, 2, bit_b3_r8},
    [0x41] = {"BIT 0 C", 2, 2, bit_b3_r8},
    [0x42] = {"BIT 0 D", 2, 2, bit_b3_r8},
    [0x43] = {"BIT 0 E", 2, 2, bit_b3_r8},
    [0x44] = {"BIT 0 H", 2, 2, bit_b3_r8},
    [0x45] = {"BIT 0 L", 2, 2, bit_b3_r8},
    [0x46] = {"BIT 0 (HL)", 2, 3, bit_b3_r8},
    [0x47] = {"BIT 0 A", 2, 2, bit_b3_r8},
    [0x48] = {"BIT 1 B", 2, 2, bit_b3_r8},
    [0x49] = {"BIT 1 C", 2, 2, bit_b3_r8},
    [0x4a] = {"BIT 1 D", 2, 2, bit_b3_r8},
    [0x4b] = {"BIT 1 E", 2, 2, bit_b3_r8},
    [0x4c] = {"BIT 1 H", 2, 2, bit_b3_r8},
    [0x4d] = {"BIT 1 L", 2, 2, bit_b3_r8},
    [0x4e] = {"BIT 1 (HL)", 2, 3, bit_b3_r8},
    [0x4f] = {"BIT 1 A", 2, 2, bit_b3_r8},
    [0x50] = {"BIT 2 B", 2, 2, bit_b3_r8},
    [0x51] = {"BIT 2 C", 2, 2, bit_b3_r8},
    [0x52] = {"BIT 2 D", 2, 2, bit_b3_r8},
    [0x53] = {"BIT 2 E", 2, 2, bit_b3_r8},
    [0x54] = {"BIT 2 H", 2, 2, bit_b3_r8},
    [0x55] = {"BIT 2 L", 2, 2, bit_b3_r8},
    [0x56] = {"BIT 2 (HL)", 2, 3, bit_b3_r8},
    [0x57] = {"BIT 2 A", 2, 2, bit_b3_r8},
    [0x58] = {"BIT 3 B", 2, 2, bit_b3_r8},
    [0x59] = {"BIT 3 C", 2, 2, bit_b3_r8},
    [0x5a] = {"BIT 3 D", 2, 2, bit_b3_r8},
    [0x5b] = {"BIT 3 E", 2, 2, bit_b3_r8},
    [0x5c] = {"BIT 3 H", 2, 2, bit_b3_r8},
    [0x5d] = {"BIT 3 L", 2, 2, bit_b3_r8},
    [0x5e] = {"BIT 3 (HL)", 2, 3, bit_b3_r8},
    [0x5f] = {"BIT 3 A", 2, 2, bit_b3_r8},
    [0x60] = {"BIT 4 B", 2, 2, bit_b3_r8},
    [0x61] = {"BIT 4 C", 2, 2, bit_b3_r8},
    [0x62] = {"BIT 4 D", 2, 2, bit_b3_r8},
    [0x63] = {"BIT 4 E", 2, 2, bit_b3_r8},
    [0x64] = {"BIT 4 H", 2, 2, bit_b3_r8},
    [0x65] = {"BIT 4 L", 2, 2, bit_b3_r8},
    [0x66] = {"BIT 4 (HL)", 2, 3, bit_b3_r8},
    [0x67] = {"BIT 4 A", 2, 2, bit_b3_r8},
    [0x68] = {"BIT 5 B", 2, 2, bit_b3_r8},
    [0x69] = {"BIT 5 C", 2, 2, bit_b3_r8},
    [0x6a] = {"BIT 5 D", 2, 2, bit_b3_r8},
    [0x6b] = {"BIT 5 E", 2, 2, bit_b3_r8},
    [0x6c] = {"BIT 5 H", 2, 2, bit_b3_r8},
    [0x6d] = {"BIT 5 L", 2, 2, bit_b3_r8},
    [0x6e] = {"BIT 5 (HL)", 2, 3, bit_b3_r8},
    [0x6f] = {"BIT 5 A", 2, 2, bit_b3_r8},
    [0x70] = {"BIT 6 B", 2, 2, bit_b3_r8},
    [0x71] = {"BIT 6 C", 2, 2, bit_b3_r8},
    [0x72] = {"BIT 6 D", 2, 2, bit_b3_r8},
    [0x73] = {"BIT 6 E", 2, 2, bit_b3_r8},
    [0x74] = {"BIT 6 H", 2, 2, bit_b3_r8},
    [0x75] = {"BIT 6 L", 2, 2, bit_b3_r8},
    [0x76] = {"BIT 6 (HL)", 2, 3, bit_b3_r8},
    [0x77] = {"BIT 6 A", 2, 2, bit_b3_r8},
    [0x78] = {"BIT 7 B", 2, 2, bit_b3_r8},
    [0x79] = {"BIT 7 C", 2, 2, bit_b3_r8},
    [0x7a] = {"BIT 7 D", 2, 2, bit_b3_r8},
    [0x7b] = {"BIT 7 E", 2, 2, bit_b3_r8},
    [0x7c] = {"BIT 7 H", 2, 2, bit_b3_r8},
    [0x7d] = {"BIT 7 L", 2, 2, bit_b3_r8},
    [0x7e] = {"BIT 7 (HL)", 2, 3, bit_b3_r8},
    [0x7f] = {"BIT 7 A", 2, 2, bit_b3_r8},
    [0x80] = {"RES 0 B", 2, 2, res_b3_r8},
    [0x81] = {"RES 0 C", 2, 2, res_b3_r8},
    [0x82] = {"RES 0 D", 2, 2, res_b3_r8},
    [0x83] = {"RES 0 E", 2, 2, res_b3_r8},
    [0x84] = {"RES 0 H", 2, 2, res_b3_r8},
    [0x85] = {"RES 0 L", 2, 2, res_b3_r8},
    [0x86] = {"RES 0 (HL)", 2, 4, res_b3_r8},
    [0x87] = {"RES 0 A", 2, 2, res_b3_r8},
    [0x88] = {"RES 1 B", 2, 2, res_b3_r8},
    [0x89] = {"RES 1 C", 2, 2, res_b3_r8},
    [0x8a] = {"RES 1 D", 2, 2, res_b3_r8},
    [0x8b] = {"RES 1 E", 2, 2, res_b3_r8},
    [0x8c] = {"RES 1 H", 2, 2, res_b3_r8},
    [0x8d] = {"RES 1 L", 2, 2, res_b3_r8},
    [0x8e] = {"RES 1 (HL)", 2, 4, res_b3_r8},
    [0x8f] = {"RES 1 A", 2, 2, res_b3_r8},
    [0x90] = {"RES 2 B", 2, 2, res_b3_r8},
    [0x91] = {"RES 2 C", 2, 2, res_b3_r8},
    [0x92] = {"RES 2 D", 2, 2, res_b3_r8},
    [0x93] = {"RES 2 E", 2, 2, res_b3_r8},
    [0x94] = {"RES 2 H", 2, 2, res_b3_r8},
    [0x95] = {"RES 2 L", 2, 2, res_b3_r8},
    [0x96] = {"RES 2 (HL)", 2, 4, res_b3_r8},
    [0x97] = {"RES 2 A", 2, 2, res_b3_r8},
    [0x98] = {"RES 3 B", 2, 2, res_b3_r8},
    [0x99] = {"RES 3 C", 2, 2, res_b3_r8},
    [0x9a] = {"RES 3 D", 2, 2, res_b3_r8},
    [0x9b] = {"RES 3 E", 2, 2, res_b3_r8},
    [0x9c] = {"RES 3 H", 2, 2, res_b3_r8},
    [0x9d] = {"RES 3 L", 2, 2, res_b3_r8},
    [0x9e] = {"RES 3 (HL)", 2, 4, res_b3_r8},
    [0x9f] = {"RES 3 A", 2, 2, res_b3_r8},
    [0xa0] = {"RES 4 B", 2, 2, res_b3_r8},
    [0xa1] = {"RES 4 C", 2, 2, res_b3_r8},
    [0xa2] = {"RES 4 D", 2, 2, res_b3_r8},
    [0xa3] = {"RES 4 E", 2, 2, res_b3_r8},
    [0xa4] = {"RES 4 H", 2, 2, res_b3_r8},
    [0xa5] = {"RES 4 L", 2, 2, res_b3_r8},
    [0xa6] = {"RES 4 (HL)", 2, 4, res_b3_r8},
    [0xa7] = {"RES 4 A", 2, 2, res_b3_r8},
    [0xa8] = {"RES 5 B", 2, 2, res_b3_r8},
    [0xa9] = {"RES 5 C", 2, 2, res_b3_r8},
    [0xaa] = {"RES 5 D", 2, 2, res_b3_r8},
    [0xab] = {"RES 5 E", 2, 2, res_b3_r8},
    [0xac] = {"RES 5 H", 2, 2, res_b3_r8},
    [0xad] = {"RES 5 L", 2, 2, res_b3_r8},
    [0xae] = {"RES 5 (HL)", 2, 4, res_b3_r8},
    [0xaf] = {"RES 5 A", 2, 2, res_b3_r8},
    [0xb0] = {"RES 6 B", 2, 2, res_b3_r8},
    [0xb1] = {"RES 6 C", 2, 2, res_b3_r8},
    [0xb2] = {"RES 6 D", 2, 2, res_b3_r8},
    [0xb3] = {"RES 6 E", 2, 2, res_b3_r8},
    [0xb4] = {"RES 6 H", 2, 2, res_b3_r8},
    [0xb5] = {"RES 6 L", 2, 2, res_b3_r8},
    [0xb6] = {"RES 6 (HL)", 2, 4, res_b3_r8},
    [0xb7] = {"RES 6 A", 2, 2, res_b3_r8},
    [0xb8] = {"RES 7 B", 2, 2, res_b3_r8},
    [0xb9] = {"RES 7 C", 2, 2, res_b3_r8},
    [0xba] = {"RES 7 D", 2, 2, res_b3_r8},
    [0xbb] = {"RES 7 E", 2, 2, res_b3_r8},
    [0xbc] = {"RES 7 H", 2, 2, res_b3_r8},
    [0xbd] = {"RES 7 L", 2, 2, res_b3_r8},
    [0xbe] = {"RES 7 (HL)", 2, 4, res_b3_r8},
    [0xbf] = {"RES 7 A", 2, 2, res_b3_r8},
    [0xc0] = {"SET 0 B", 2, 2, set_b3_r8},
    [0xc1] = {"SET 0 C", 2, 2, set_b3_r8},
    [0xc2] = {"SET 0 D", 2, 2, set_b3_r8},
    [0xc3] = {"SET 0 E", 2, 2, set_b3_r8},
    [0xc4] = {"SET 0 H", 2, 2, set_b3_r8},
    [0xc5] = {"SET 0 L", 2, 2, set_b3_r8},
    [0xc6] = {"SET 0 (HL)", 2, 4, set_b3_r8},
    [0xc7] = {"SET 0 A", 2, 2, set_b3_r8},
    [0xc8] = {"SET 1 B", 2, 2, set_b3_r8},
    [0xc9] = {"SET 1 C", 2, 2, set_b3_r8},
    [0xca] = {"SET 1 D", 2, 2, set_b3_r8},
    [0xcb] = {"SET 1 E", 2, 2, set_b3_r8},
    [0xcc] = {"SET 1 H", 2, 2, set_b3_r8},
    [0xcd] = {"SET 1 L", 2, 2, set_b3_r8},
    [0xce] = {"SET 1 (HL)", 2, 4, set_b3_r8},
    [0xcf] = {"SET 1 A", 2, 2, set_b3_r8},
    [0xd0] = {"SET 2 B", 2, 2, set_b3_r8},
    [0xd1] = {"SET 2 C", 2, 2, set_b3_r8},
    [0xd2] = {"SET 2 D", 2, 2, set_b3_r8},
    [0xd3] = {"SET 2 E", 2, 2, set_b3_r8},
    [0xd4] = {"SET 2 H", 2, 2, set_b3_r8},
    [0xd5] = {"SET 2 L", 2, 2, set_b3_r8},
    [0xd6] = {"SET 2 (HL)", 2, 4, set_b3_r8},
    [0xd7] = {"SET 2 A", 2, 2, set_b3_r8},
    [0xd8] = {"SET 3 B", 2, 2, set_b3_r8},
    [0xd9] = {"SET 3 C", 2, 2, set_b3_r8},
    [0xda] = {"SET 3 D", 2, 2, set_b3_r8},
    [0xdb] = {"SET 3 E", 2, 2, set_b3_r8},
    [0xdc] = {"SET 3 H", 2, 2, set_b3_r8},
    [0xdd] = {"SET 3 L", 2, 2, set_b3_r8},
    [0xde] = {"SET 3 (HL)", 2, 4, set_b3_r8},
    [0xdf] = {"SET 3 A", 2, 2, set_b3_r8},
    [0xe0] = {"SET 4 B", 2, 2, set_b3_r8},
    [0xe1] = {"SET 4 C", 2, 2, set_b3_r8},
    [0xe2] = {"SET 4 D", 2, 2, set_b3_r8},
    [0xe3] = {"SET 4 E", 2, 2, set_b3_r8},
    [0xe4] = {"SET 4 H", 2, 2, set_b3_r8},
    [0xe5] = {"SET 4 L", 2, 2, set_b3_r8},
    [0xe6] = {"SET 4 (HL)", 2, 4, set_b3_r8},
    [0xe7] = {"SET 4 A", 2, 2, set_b3_r8},
    [0xe8] = {"SET 5 B", 2, 2, set_b3_r8},
    [0xe9] = {"SET 5 C", 2, 2, set_b3_r8},
    [0xea] = {"SET 5 D", 2, 2, set_b3_r8},
    [0xeb] = {"SET 5 E", 2, 2, set_b3_r8},
    [0xec] = {"SET 5 H", 2, 2, set_b3_r8},
    [0xed] = {"SET 5 L", 2, 2, set_b3_r8},
    [0xee] = {"SET 5 (HL)", 2, 4, set_b3_r8},
    [0xef] = {"SET 5 A", 2, 2, set_b3_r8},
    [0xf0] = {"SET 6 B", 2, 2, set_b3_r8},
    [0xf1] = {"SET 6 C", 2, 2, set_b3_r8},
    [0xf2] = {"SET 6 D", 2, 2, set_b3_r8},
    [0xf3] = {"SET 6 E", 2, 2, set_b3_r8},
    [0xf4] = {"SET 6 H", 2, 2, set_b3_r8},
    [0xf5] = {"SET 6 L", 2, 2, set_b3_r8},
    [0xf6] = {"SET 6 (HL)", 2, 4, set_b3_r8},
    [0xf7] = {"SET 6 A", 2, 2, set_b3_r8},
    [0xf8] = {"SET 7 B", 2, 2, set_b3_r8},
    [0xf9] = {"SET 7 C", 2, 2, set_b3_r8},
    [0xfa] = {"SET 7 D", 2, 2, set_b3_r8},
    [0xfb] = {"SET 7 E", 2, 2, set_b3_r8},
    [0xfc] = {"SET 7 H", 2, 2, set_b3_r8},
    [0xfd] = {"SET 7 L", 2, 2, set_b3_r8},
    [0xfe] = {"SET 7 (HL)", 2, 4, set_b3_r8},
    [0xff] = {"SET 7 A", 2, 2, set_b3_r8},
};
// clang-format on