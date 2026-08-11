#include "emu.h"

#include "cart.h"
#include "cpu.h"
#include "mem.h"
#include "ppu.h"
#include "renderer.h"

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
    cpu_step();
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