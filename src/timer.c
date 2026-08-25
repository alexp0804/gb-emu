#include "timer.h"

#include "common.h"
#include "cpu.h"

const u16 timer_select_values[4] = {256, 4, 16, 64};
timer_s timer;

void timer_write(u16 addr, u8 val) {
    switch (addr) {
        case DIV_REG:
            timer.div = 0;
            break;
        case TIMA_REG:
            timer.tima = val;
            break;
        case TMA_REG:
            timer.tma = val;
            break;
        case TAC_REG:
            timer.clock_select = val & 0b11;
            timer.clock_enable = TEST_BIT(val, 2);
            break;
    }
}
u8 timer_read(u16 addr) {
    switch (addr) {
        case DIV_REG:
            return timer.div;
        case TIMA_REG:
            return timer.tima;
        case TMA_REG:
            return timer.tma;
        case TAC_REG:
            return (timer.clock_enable << 2) | (timer.clock_select & 0b11);
    }
    __builtin_unreachable();
}

void timer_init(void) {
    timer = (timer_s){
        .cycles_since_div_tick = 0,
        .cycles_since_tima_tick = 0,
        .div = 0x18,
        .tima = 0x00,
        .tma = 0x00,
        .clock_enable = 0,
        .clock_select = 0,
    };
}

static void div_step(u8 cycles) {
    timer.cycles_since_div_tick += cycles;
    u16 period = 256;

    while (timer.cycles_since_div_tick >= period) {
        timer.cycles_since_div_tick -= period;
        timer.div++;
    }
}
static void tima_step(u8 cycles) {
    if (!timer.clock_enable)
        return;

    timer.cycles_since_tima_tick += cycles;

    u16 period = timer_select_values[timer.clock_select];
    while (timer.cycles_since_tima_tick >= period) {
        timer.cycles_since_tima_tick -= period;
        timer.tima++;

        if (timer.tima == 0) {
            timer.tima = timer.tma;
            cpu_request_interrupt(INT_TIMER);
        }
    }
}

void timer_step(u8 cycles) {
    div_step(cycles);
    tima_step(cycles);
}