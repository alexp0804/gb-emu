#include "emu.h"

#include "cpu.h"
#include "mem.h"

void emu_init() { cpu_init(); }
void emu_step() { cpu_step(); }

bool read_rom_file(char* rom_file) {
    char path_to_rom[256];
    snprintf(path_to_rom, sizeof(path_to_rom), "roms/%s", rom_file);
    FILE* fptr = fopen(path_to_rom, "rb");
    if (!fptr) {
        perror("Error opening file");
        return false;
    }
    fread(cartridge, 1, MAX_CART_SIZE, fptr);
    fclose(fptr);
    return true;
}