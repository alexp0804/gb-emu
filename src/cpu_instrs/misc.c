#include "common.h"
#include "cpu.h"
#include "cpu_impl.h"

void unimplemented(void) {
    printf("Unimplemented instruction!\n");
    exit(1);
}

void noop(void) {}