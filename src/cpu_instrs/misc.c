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