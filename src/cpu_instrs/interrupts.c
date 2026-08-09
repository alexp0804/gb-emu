#include "common.h"
#include "cpu.h"
#include "cpu_impl.h"

void di() { cpu.interrupt_master_enable = false; }

void ei() { cpu.interrupt_master_enable = true; }