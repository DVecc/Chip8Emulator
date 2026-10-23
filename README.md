# CHIP-8 Emulator

A Windows-only C++17 CHIP-8 interpreter with Vulkan rendering, native Win32 keyboard input, and working miniaudio sound. Compatibility is partial; the project does not yet implement complete CHIP-8 compatibility.

![Pong running in the actual emulator](docs/media/pong-gameplay.jpg)

[Watch the 6.2-second gameplay recording](docs/media/pong-gameplay.gif)

The screenshot and silent GIF are real window captures of the local one-player Pong ROM, not a mockup. ROM files are not included in the demo media folder. See [validation notes](docs/validation.md) for what was tested and what remains unfinished.

## Build and run

Requires Windows x64, Visual Studio 2022 with **Desktop development with C++**, a Windows SDK, CMake 3.22 or later, and a Vulkan SDK containing `glslc`. The graphics driver must support Vulkan. GLM is in `glm/`; the preserved local miniaudio header is in `src/utils/miniaudio.h` and retains its license notices. The CMake build does not need the old CppWinRT NuGet package.

Run these commands from the repository root in PowerShell. Set `VULKAN_SDK` to your installed SDK directory; the value below is the exact version used for verification.

```powershell
$env:VULKAN_SDK = 'C:\VulkanSDK\1.4.309.0'
cmake -S . -B out -G 'Visual Studio 17 2022' -A x64
cmake --build out --config Release
ctest --test-dir out -C Release --output-on-failure
& .\out\Release\Chip8Emulator.exe 'C:\path\to\your\licensed-game.ch8'
```

The ROM path is required. Quote paths containing spaces. Relative ROM paths resolve from the directory where you invoke the program. Shaders compile during the build and are copied beside the executable under `shaders/`; runtime shader lookup uses the executable directory. Copy that folder with the executable if moving the build. ROMs are deliberately not copied into build output.

For this machine's local playtest, the exact launch command was:

```powershell
& .\out\Release\Chip8Emulator.exe '.\src\Pong (1 player).ch8'
```

The existing Visual Studio solution also supports **Release / x64** after setting `VULKAN_SDK`. Its include paths now use `$(ProjectDir)` and `$(VULKAN_SDK)`, and its post-build step compiles and places the shaders. The canonical tested workflow is CMake above. Win32/x86 is not verified; use x64 with the SDK's `Lib` directory.

## Controls and implementation

The PC keypad maps to CHIP-8 like this:

```text
PC             CHIP-8
1 2 3 4        1 2 3 C
Q W E R        4 5 6 D
A S D F        7 8 9 E
Z X C V        A 0 B F
```

Release Escape to exit. Game-specific controls vary.

`src/emulator/` holds the registers, 4 KB memory, opcode dispatch, 64 x 32 framebuffer, and audio. Sprite drawing uses XOR and reports collisions. `src/renderer/` uploads the framebuffer to a Vulkan texture and presents it. `src/windows_platform/` translates Win32 keyboard events. `src/main.cpp` loads the selected ROM and runs ten instructions per approximately 60 Hz frame; timer registers tick once per frame rather than once per instruction.

This is a learning project, not a claim of complete CHIP-8 conformance. Timing can drift under load, the display stretches to the window, and interpreter quirks are fixed rather than configurable. The core is not a sandbox for untrusted ROMs. Audio works, as confirmed by the developer. Full game controls and the entire local ROM collection have not been validated.

## ROMs and repository history

ROMs are not packaged with the executable. Supply your own ROM path when launching. This update adds no ROM files and does not change the existing ROM files; newly added `*.ch8` files are ignored. Some ROMs already exist in the repository's older history. Gameplay captures are included for the portfolio demonstration.

The emulator is on `main`. The earlier main history is preserved as `archive/main-before-emulator-2026-10-22`; the original emulator branch and existing `portfolio/*` branches remain available.
