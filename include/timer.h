#pragma once
#include "common.h"

#define DIV_REG 0xFF04
#define TIMA_REG 0xFF05
#define TMA_REG 0xFF06
#define TAC_REG 0xFF07

const u16 clock_select_values[4];

typedef struct timer {
    u8 div, tma, tima;
    u8 clock_select;
    bool clock_enable;
    u16 cycles_since_div_tick, cycles_since_tima_tick;
} timer_s;

extern timer_s timer;

void timer_write(u16 addr, u8 val);
u8 timer_read(u16 addr);
void timer_init(void);
void timer_step(u8 cycles);