#include "cpu.h"

#include "cpu_impl.h"
#include "mem.h"

cpu_s cpu;

// Todo: in the case of branching instructions, cycle lengths may be different based on if the
// condition for that instruction is met or not.

void cpu_init(void) {
    cpu.reg = (reg_s){
        .a = 0x01,
        .f = 0x80,
        .b = 0x00,
        .c = 0x13,
        .d = 0x00,
        .e = 0xd8,
        .h = 0x01,
        .l = 0x4d,
        .pc = 0x0100,
        .sp = 0xfffe,
    };
}

void debug_register() {
    printf("pc=0x%04x\n", cpu.reg.pc);
    printf("op=0x%04x\n", cpu.opcode);
    printf("af=0x%04x\n", cpu.reg.af);
    printf("bc=0x%04x\n", cpu.reg.bc);
    printf("de=0x%04x\n", cpu.reg.de);
    printf("hl=0x%04x\n", cpu.reg.hl);
    printf("sp=0x%04x\n", cpu.reg.sp);
    printf("\n");
}

u8 cpu_step(void) {
    cpu.opcode = mem_read(cpu.reg.pc);
    // debug_register();
    cpu.reg.pc++;
    instruction_s instr = decode_opcode();
    instr.execute();

    // getchar();

    return instr.cycles;
}

u8 fetch_byte(void) { return mem_read(cpu.reg.pc++); }
u16 fetch_word(void) {
    u8 lo = fetch_byte();
    u8 hi = fetch_byte();
    return (hi << 8) | lo;
}

void push_word(u16 val) {
    mem_write(--cpu.reg.sp, (val & 0xFF00) >> 8);
    mem_write(--cpu.reg.sp, (val & 0x00FF));
}
u16 pop_word() {
    u16 lo = mem_read(cpu.reg.sp++);
    u16 hi = mem_read(cpu.reg.sp++);

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
            mem_write(cpu.reg.hl, val);
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
            return mem_read(cpu.reg.hl);
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