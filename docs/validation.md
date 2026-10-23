# Verification and demo checklist

## Checked against the source

- Emulator source exists on `Chip8-Rendering`; `main` is an earlier Pong implementation.
- `main.cpp` drives instruction cycles and Vulkan rendering; ROM selection is hard-coded.
- `win32_platform.cpp` maps sixteen keyboard keys to the CHIP-8 keypad.
- `cycle()` decrements timer registers after each instruction, not on a separate 60 Hz tick.
- Build and debug shader compilation reference machine-specific absolute paths.

## Not yet verified

- Clean Windows build, runtime launch, Vulkan validation-layer output, audible sound.
- Correctness across opcode/quirk test ROMs or reliable behavior with malformed input.
- Resize, minimize/restore, and close behavior under repeated use.

## Concrete code-review concerns to investigate

1. `LoadROM` copies bytes without checking that the ROM fits in the remaining RAM.
2. `TableF` indexes a 0x66-entry table with an arbitrary low opcode byte; invalid opcodes
   can index beyond the table. Some other instruction families have similar bounds concerns.
3. Stack underflow/overflow and program-counter bounds are not explicitly guarded.
4. Subtraction's no-borrow flag on equal operands should be checked against the chosen
   CHIP-8 specification. Do not claim a fix until its tests and quirk policy are explicit.
5. Timer scheduling currently follows CPU execution speed. Separate instruction rate
   and timer ticks and test them independently of rendering.

These are review findings, not changes made by this documentation update.

## Record a portfolio demo on your Windows machine

1. Show the branch name and a successful fresh build.
2. Run a ROM for which you have permission to show gameplay, with the keypad overlay visible.
3. Show input, a resize, and a clean exit. Record 20–30 seconds.
4. Save a clear still frame as `docs/gameplay.png`, embed it in the README, and attach
   the recording to a GitHub release. Do not substitute a mockup for a real capture.
5. Record your OS, GPU, Vulkan SDK, compiler and exact commit in the release notes.
6. Add individual passing opcode tests before claiming accuracy or compatibility.

## Interview prompts

Explain how an opcode reaches its handler; how Dxyn modifies the framebuffer; why
rendering and emulation clocks should be separate; how input reaches Fx0A; and how
you would make the CPU testable without opening a Vulkan window.
