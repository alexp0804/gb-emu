#pragma once

#include "common.h"

#define CLOCK_SPEED 4194304

#define IF_REG 0xFF0F
#define IE_REG 0xFFFF

#define INT_VBLANK_HANDLER 0x40
#define INT_LCD_HANDLER 0x48
#define INT_TIMER_HANDLER 0x50
#define INT_SERIAL_HANDLER 0x58
#define INT_JOYPAD_HANDLER 0x60

typedef enum interrupt { INT_VBLANK, INT_LCD, INT_TIMER, INT_SERIAL, INT_JOYPAD } interrupt_e;

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
    u16 sp, pc;
} reg_s;

typedef struct cpu {
    reg_s reg;
    u8 opcode;
    u8 interrupt_flag, interrupt_enable;
    bool interrupt_master_enable;
    u8 extra_cycles;
} cpu_s;

extern cpu_s cpu;

void cpu_request_interrupt(interrupt_e i);
void cpu_handle_interrupts();
void cpu_init(void);
u8 cpu_step(void);