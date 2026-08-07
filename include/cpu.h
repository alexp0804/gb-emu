#pragma once

#include "common.h"

typedef struct reg {
    union {
        struct {
            u8 f;
            u8 a;
        };
        u16 af;
    };
    union {
        struct {
            u8 c;
            u8 b;
        };
        u16 bc;
    };
    union {
        struct {
            u8 e;
            u8 d;
        };
        u16 de;
    };
    union {
        struct {
            u8 l;
            u8 h;
        };
        u16 hl;
    };
} reg_s;

typedef struct cpu {
    reg_s reg;
    u8 sp, pc;
    u8 opcode;
} cpu_s;

extern cpu_s cpu;

void cpu_init(void);
u8 cpu_step(void);