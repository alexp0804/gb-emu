#include "cpu.h"

#include "cpu_impl.h"
#include "mem.h"

cpu_s cpu;

u8 fetch_byte(void) { return read_mem(cpu.reg.pc++); }
u16 fetch_word(void) {
    u8 lo = fetch_byte();
    u8 hi = fetch_byte();
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
    [0x03] = {"", 0, 0, unimplemented},
    [0x04] = {"", 0, 0, unimplemented},
    [0x05] = {"", 0, 0, unimplemented},
    [0x06] = {"LD B, n8", 2, 2, ld_r8_n8},
    [0x07] = {"", 0, 0, unimplemented},
    [0x08] = {"", 0, 0, unimplemented},
    [0x09] = {"", 0, 0, unimplemented},
    [0x0a] = {"LD A, (BC)", 1, 2, ld_a_at_r16mem},
    [0x0b] = {"", 0, 0, unimplemented},
    [0x0c] = {"", 0, 0, unimplemented},
    [0x0d] = {"", 0, 0, unimplemented},
    [0x0e] = {"LD C, n8", 2, 2, ld_r8_n8},
    [0x0f] = {"", 0, 0, unimplemented},
    [0x10] = {"", 0, 0, unimplemented},
    [0x11] = {"LD DE, n16", 3, 3, ld_r16_n16},
    [0x12] = {"LD (DE), A", 1, 2, ld_at_r16mem_a},
    [0x13] = {"", 0, 0, unimplemented},
    [0x14] = {"", 0, 0, unimplemented},
    [0x15] = {"", 0, 0, unimplemented},
    [0x16] = {"LD D, n8", 2, 2, ld_r8_n8},
    [0x17] = {"", 0, 0, unimplemented},
    [0x18] = {"", 0, 0, unimplemented},
    [0x19] = {"", 0, 0, unimplemented},
    [0x1a] = {"LD A, (DE)", 1, 2, ld_a_at_r16mem},
    [0x1b] = {"", 0, 0, unimplemented},
    [0x1c] = {"", 0, 0, unimplemented},
    [0x1d] = {"", 0, 0, unimplemented},
    [0x1e] = {"LD E, n8", 2, 2, ld_r8_n8},
    [0x1f] = {"", 0, 0, unimplemented},
    [0x20] = {"", 0, 0, unimplemented},
    [0x21] = {"LD HL, n16", 3, 3, ld_r16_n16},
    [0x22] = {"LD (HL+), A", 1, 2, ld_at_r16mem_a},
    [0x23] = {"", 0, 0, unimplemented},
    [0x24] = {"", 0, 0, unimplemented},
    [0x25] = {"", 0, 0, unimplemented},
    [0x26] = {"LD H, n8", 2, 2, ld_r8_n8},
    [0x27] = {"", 0, 0, unimplemented},
    [0x28] = {"", 0, 0, unimplemented},
    [0x29] = {"", 0, 0, unimplemented},
    [0x2a] = {"LD A, (HL+)", 1, 2, ld_a_at_r16mem},
    [0x2b] = {"", 0, 0, unimplemented},
    [0x2c] = {"", 0, 0, unimplemented},
    [0x2d] = {"", 0, 0, unimplemented},
    [0x2e] = {"LD L, n8", 2, 2, ld_r8_n8},
    [0x2f] = {"", 0, 0, unimplemented},
    [0x30] = {"", 0, 0, unimplemented},
    [0x31] = {"LD SP, n16", 3, 3, ld_r16_n16},
    [0x32] = {"LD (HL-), A", 1, 2, ld_at_r16mem_a},
    [0x33] = {"", 0, 0, unimplemented},
    [0x34] = {"", 0, 0, unimplemented},
    [0x35] = {"", 0, 0, unimplemented},
    [0x36] = {"LD (HL), n8", 2, 2, ld_r8_n8},
    [0x37] = {"", 0, 0, unimplemented},
    [0x38] = {"", 0, 0, unimplemented},
    [0x39] = {"", 0, 0, unimplemented},
    [0x3a] = {"LD A (HL-)", 1, 2, ld_a_at_r16mem},
    [0x3b] = {"", 0, 0, unimplemented},
    [0x3c] = {"", 0, 0, unimplemented},
    [0x3d] = {"", 0, 0, unimplemented},
    [0x3e] = {"LD A, n8", 2, 2, ld_r8_n8},
    [0x3f] = {"", 0, 0, unimplemented},
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
    [0xa0] = {"", 0, 0, unimplemented},
    [0xa1] = {"", 0, 0, unimplemented},
    [0xa2] = {"", 0, 0, unimplemented},
    [0xa3] = {"", 0, 0, unimplemented},
    [0xa4] = {"", 0, 0, unimplemented},
    [0xa5] = {"", 0, 0, unimplemented},
    [0xa6] = {"", 0, 0, unimplemented},
    [0xa7] = {"", 0, 0, unimplemented},
    [0xa8] = {"", 0, 0, unimplemented},
    [0xa9] = {"", 0, 0, unimplemented},
    [0xaa] = {"", 0, 0, unimplemented},
    [0xab] = {"", 0, 0, unimplemented},
    [0xac] = {"", 0, 0, unimplemented},
    [0xad] = {"", 0, 0, unimplemented},
    [0xae] = {"", 0, 0, unimplemented},
    [0xaf] = {"", 0, 0, unimplemented},
    [0xb0] = {"", 0, 0, unimplemented},
    [0xb1] = {"", 0, 0, unimplemented},
    [0xb2] = {"", 0, 0, unimplemented},
    [0xb3] = {"", 0, 0, unimplemented},
    [0xb4] = {"", 0, 0, unimplemented},
    [0xb5] = {"", 0, 0, unimplemented},
    [0xb6] = {"", 0, 0, unimplemented},
    [0xb7] = {"", 0, 0, unimplemented},
    [0xb8] = {"", 0, 0, unimplemented},
    [0xb9] = {"", 0, 0, unimplemented},
    [0xba] = {"", 0, 0, unimplemented},
    [0xbb] = {"", 0, 0, unimplemented},
    [0xbc] = {"", 0, 0, unimplemented},
    [0xbd] = {"", 0, 0, unimplemented},
    [0xbe] = {"", 0, 0, unimplemented},
    [0xbf] = {"", 0, 0, unimplemented},
    [0xc0] = {"", 0, 0, unimplemented},
    [0xc1] = {"", 0, 0, unimplemented},
    [0xc2] = {"", 0, 0, unimplemented},
    [0xc3] = {"", 0, 0, unimplemented},
    [0xc4] = {"", 0, 0, unimplemented},
    [0xc5] = {"", 0, 0, unimplemented},
    [0xc6] = {"", 0, 0, unimplemented},
    [0xc7] = {"", 0, 0, unimplemented},
    [0xc8] = {"", 0, 0, unimplemented},
    [0xc9] = {"", 0, 0, unimplemented},
    [0xca] = {"", 0, 0, unimplemented},
    [0xcb] = {"", 0, 0, unimplemented},
    [0xcc] = {"", 0, 0, unimplemented},
    [0xcd] = {"", 0, 0, unimplemented},
    [0xce] = {"", 0, 0, unimplemented},
    [0xcf] = {"", 0, 0, unimplemented},
    [0xd0] = {"", 0, 0, unimplemented},
    [0xd1] = {"", 0, 0, unimplemented},
    [0xd2] = {"", 0, 0, unimplemented},
    [0xd3] = {"", 0, 0, unimplemented},
    [0xd4] = {"", 0, 0, unimplemented},
    [0xd5] = {"", 0, 0, unimplemented},
    [0xd6] = {"", 0, 0, unimplemented},
    [0xd7] = {"", 0, 0, unimplemented},
    [0xd8] = {"", 0, 0, unimplemented},
    [0xd9] = {"", 0, 0, unimplemented},
    [0xda] = {"", 0, 0, unimplemented},
    [0xdb] = {"", 0, 0, unimplemented},
    [0xdc] = {"", 0, 0, unimplemented},
    [0xdd] = {"", 0, 0, unimplemented},
    [0xde] = {"", 0, 0, unimplemented},
    [0xdf] = {"", 0, 0, unimplemented},
    [0xe0] = {"", 0, 0, unimplemented},
    [0xe1] = {"", 0, 0, unimplemented},
    [0xe2] = {"", 0, 0, unimplemented},
    [0xe3] = {"", 0, 0, unimplemented},
    [0xe4] = {"", 0, 0, unimplemented},
    [0xe5] = {"", 0, 0, unimplemented},
    [0xe6] = {"", 0, 0, unimplemented},
    [0xe7] = {"", 0, 0, unimplemented},
    [0xe8] = {"", 0, 0, unimplemented},
    [0xe9] = {"", 0, 0, unimplemented},
    [0xea] = {"LD (n16), A", 3, 4, ld_at_n16_a},
    [0xeb] = {"", 0, 0, unimplemented},
    [0xec] = {"", 0, 0, unimplemented},
    [0xed] = {"", 0, 0, unimplemented},
    [0xee] = {"", 0, 0, unimplemented},
    [0xef] = {"", 0, 0, unimplemented},
    [0xf0] = {"", 0, 0, unimplemented},
    [0xf1] = {"", 0, 0, unimplemented},
    [0xf2] = {"", 0, 0, unimplemented},
    [0xf3] = {"", 0, 0, unimplemented},
    [0xf4] = {"", 0, 0, unimplemented},
    [0xf5] = {"", 0, 0, unimplemented},
    [0xf6] = {"", 0, 0, unimplemented},
    [0xf7] = {"", 0, 0, unimplemented},
    [0xf8] = {"", 0, 0, unimplemented},
    [0xf9] = {"", 0, 0, unimplemented},
    [0xfa] = {"LD A, (n16)", 3, 4, ld_a_at_n16},
    [0xfb] = {"", 0, 0, unimplemented},
    [0xfc] = {"", 0, 0, unimplemented},
    [0xfd] = {"", 0, 0, unimplemented},
    [0xfe] = {"", 0, 0, unimplemented},
    [0xff] = {"", 0, 0, unimplemented},
};
const instruction_s bit_shift_instruction_table[8] = {
    [0x0] = {"", 0, 0, unimplemented},
    [0x1] = {"", 0, 0, unimplemented},
    [0x2] = {"", 0, 0, unimplemented},
    [0x3] = {"", 0, 0, unimplemented},
    [0x4] = {"", 0, 0, unimplemented},
    [0x5] = {"", 0, 0, unimplemented},
    [0x6] = {"", 0, 0, unimplemented},
    [0x7] = {"", 0, 0, unimplemented},
};
const instruction_s bit_flag_instruction_table[4] = {
    [0x0] = {"", 0, 0, unimplemented},
    [0x1] = {"", 0, 0, unimplemented},
    [0x2] = {"", 0, 0, unimplemented},
    [0x3] = {"", 0, 0, unimplemented},
};
// clang-format on