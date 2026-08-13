#include "common.h"
#include "cpu.h"
#include "cpu_impl.h"

void unimplemented() {
    printf("Unimplemented instruction!\n");
    exit(1);
}
void invalid() {
    printf("Encountered an invalid instruction!\n");
    exit(1);
}

void daa() {
    if (!FLAG_IS_SET(FLAG_N)) {
        if (FLAG_IS_SET(FLAG_C) || cpu.reg.a > 0x99) {
            cpu.reg.a += 0x60;
            FLAG_SET(FLAG_C);
        }
        if (FLAG_IS_SET(FLAG_H) || (cpu.reg.a & 0x0F) > 0x09)
            cpu.reg.a += 0x6;
    } else {
        if (FLAG_IS_SET(FLAG_C))
            cpu.reg.a -= 0x60;
        if (FLAG_IS_SET(FLAG_H))
            cpu.reg.a -= 0x6;
    }
    FLAG_WRITE(FLAG_Z, cpu.reg.a == 0);
    FLAG_CLEAR(FLAG_H);
}

void noop() {
}

void stop() {
    // TODO
}

void ccf() {
    FLAG_CLEAR(FLAG_N | FLAG_H);
    FLAG_WRITE(FLAG_C, !FLAG_IS_SET(FLAG_C));
}

void scf() {
    FLAG_CLEAR(FLAG_N | FLAG_H);
    FLAG_SET(FLAG_C);
}