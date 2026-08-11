#include "emu.h"

#include "cart.h"
#include "cpu.h"
#include "mem.h"
#include "ppu.h"
#include "renderer.h"

const u32 cycles_per_frame = CLOCK_SPEED / FRAME_RATE;

void emu_init() {
    renderer_init();
    cpu_init();
    mem_init();
}
void emu_deinit() {
    renderer_deinit();
    mem_deinit();
}
void emu_step() {
    u32 cycles_this_frame = 0;
    while (cycles_this_frame < cycles_per_frame) {
        u8 cycles = cpu_step();
        cycles_this_frame += cycles;
        ppu_step(cycles);
        cpu_handle_interrupts();
    }
    renderer_step();
}

bool read_rom_file(char* rom_file) {
    char path_to_rom[256];
    snprintf(path_to_rom, sizeof(path_to_rom), "roms/%s", rom_file);
    FILE* fptr = fopen(path_to_rom, "rb");
    if (!fptr) {
        perror("Error opening file");
        return false;
    }
    fread(cart.rom, 1, MAX_CART_ROM_SIZE, fptr);
    fclose(fptr);
    return true;
}