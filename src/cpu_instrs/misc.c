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

void noop() {}

void ccf() {
    FLAG_CLEAR(FLAG_N | FLAG_H);
    FLAG_WRITE(FLAG_C, !FLAG_IS_SET(FLAG_C));
}

void scf() {
    FLAG_CLEAR(FLAG_N | FLAG_H);
    FLAG_SET(FLAG_C);
}