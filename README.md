# gb-emu
A gameboy emulator written in C.

TODO: update with screenshots of games that work.

## Features
- Scanline based PPU rendering, passing all of [dmg-acid2](https://github.com/mattcurrie/dmg-acid2) tests.
- Supports cartridges with MBC1 <!-- and MBC3 --> memory bank controllers.

## Requirements
- GCC is required to compile the program.
- SDL is required for graphics and input.

## Build
- Run `make`.
- Remove build artifacts and binaries with `make clean`.

## Usage
The emulator expects to find ROM files within the `roms/` directory of the project root.

##### Example:
Place your legally obtained ROM files into the `roms/` directory.
```
    src/...
    include/...
    roms/
      - Tetris.gb
    Makefile
    ...
```
Then run with: `./bin/gb-emu Tetris.gb`

If no ROM file is specified, the emulator will try to load `Tetris.gb`.

Input is simply mapped to the keyboard.
| Gameboy Input | Keyboard Mapping |
| ------ | ------ |
| D-Pad | WASD |
| A |  J |
| B | K |
| Start | Enter |
| Select | Backspace |


## Limitations and future improvements
- STOP and HALT mode are unimplemented.
- Audio remains unimplemented.
- Cartridges with MBC types other than MBC1 are not supported.
- Some specific quirks of the Gameboy remain unimplemented, like the OAM corruption bug, the HALT bug, and other small behaviors like the TIMA register getting the wrong value if TMA is updated on the same clock cycle.
- Controller support.
- Support for Gameboy Color games.