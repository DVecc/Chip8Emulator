# Build and validation record

Test date: October 22, 2026 (America/New_York).

## Environment and baseline

- Windows x64; Visual Studio 2022 Community, MSVC 19.42.34438, Windows SDK 10.0.22621.0.
- CMake 3.22.1 and Vulkan SDK 1.4.309.0 already installed. No system-wide packages needed installation.
- Desktop `Chip8-Rendering` matched GitHub commit `8d854a4`; its four modified files and untracked `miniaudio.h` made the working tree newer than that commit. No repository AGENTS.md or README existed on the checked-out emulator branch. The remote presentation README was reviewed after inspecting local source.

## Changes made

- Added CMake configuration, build-time shader compilation, executable-relative shader lookup, and a required command-line ROM path. Removed runtime shell invocation of a hardcoded shader compiler.
- Made Visual Studio include/library paths relative or `VULKAN_SDK` based, enabled C++17 in Release, and removed unused mandatory CppWinRT restore imports. Added shader post-build commands.
- Fixed Release startup: the debug messenger is created only when validation layers and their extension are enabled.
- Fixed the UV vertex attribute from three floats to two floats.
- Split timer updates from instruction execution; ten CPU instructions now share one timer tick.
- Added ROM length and PC-fetch bounds checks. Filled dispatch arrays across their complete index ranges so unsupported entries fail with an exception.
- Added a defined failure path for unavailable Vulkan memory types, corrected swapped resize width/height arguments, skipped zero-sized windows in the main loop, and added a brief idle sleep.
- Preserved local miniaudio/audio and keyboard changes. Logged messages now also go to stderr for diagnosing redirected launches.

## Verification

The README contains the exact CMake commands. Release / x64 compiled and linked successfully, including both GLSL shaders. CTest's `chip8_core` check passed. It uses tiny instruction programs authored for the test, generates a temporary binary outside the repository, and removes it afterward; it does not depend on third-party ROMs.

The regression check exercises:

- Built-in font drawing, XOR erasure, and the collision flag through conditional execution.
- CPU cycles leaving the delay register unchanged until an explicit timer tick.
- Key-wait completion and font lookup from a staged keypress.
- Oversized ROM rejection (3585 bytes; valid capacity is 3584 bytes).

Manual runtime observations:

- Rush Hour title rendered in a real Win32 window. A mapped keyboard press changed its selection display. A complete puzzle playthrough was not tested.
- One-player Pong rendered paddles, an animated ball, and changing scores. The score advanced during observation. This establishes active emulation and presentation, not complete game correctness.
- Escape closed each visible playtest window. Missing arguments printed usage; a nonexistent ROM printed a load error and exited.
- Actual window captures produced `media/pong-gameplay.jpg` and `media/pong-gameplay.gif`. The GIF contains 30 captured frames (23 distinct images) over approximately 6.2 seconds, with capture intervals retained. It is silent and sampled at about 5 frames per second.

## Remaining limitations

- No complete opcode conformance suite, SCHIP/XO-CHIP support claim, or configurable interpreter quirks. Subtraction borrow equality and flag/register alias behavior need further review.
- ROM length and fetch checks are partial hardening. Stack accesses, sprite/data addresses, and ROM-selected keypad indexes still require additional bounds checks. Use trusted ROMs only.
- Timers are frame paced, with no accumulator or catch-up under load. Approximate 600 instructions/second is fixed. Input taps shorter than a CPU update may be missed.
- Audio works, confirmed by the developer during the local playtest. The capture is silent; sound-timer duration and audio-device compatibility have not been separately measured.
- Window resize/minimize handling was inspected and corrected where noted, but repeated resize, minimize/restore, high-DPI displays, other GPUs, and device-loss recovery are not comprehensively tested.
- Display aspect ratio follows the window rather than maintaining the CHIP-8 2:1 ratio. Texture upload allocates staging resources each frame and is not optimized.
- Debug validation-layer runs and a clean-machine dependency installation are not verified. Existing GPU error handling and partial-initialization cleanup warrant further work.
- ROMs are not packaged with the emulator. This update adds or modifies no ROM content. Existing historical ROM files are not part of a new release package.
- Windows x64 is the supported and tested platform. Complete CHIP-8 compatibility is not implemented. The Desktop working tree was preserved while fixes, tests, and demo media were prepared in a separate working copy.

The legacy Visual Studio solution initially failed with `LNK1101: incorrect MSPDB140.DLL version` when using the default host tools. Building with the x64 host tools succeeded:

```powershell
$env:VULKAN_SDK = 'C:\VulkanSDK\1.4.309.0'
& 'C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe' Chip8Emulator.sln /p:Configuration=Release /p:Platform=x64 /p:PreferredToolArchitecture=x64 /p:OutDir=vs-out64\ /p:IntDir=vs-out64\obj\ /verbosity:minimal /nologo
```

`PreferredToolArchitecture=x64` is now recorded in the project. This run emitted the existing nonfatal `LNK4075` warning because Release enables optimization alongside incremental linking. Both shader outputs were generated in `vs-out64/shaders/`.

## Portfolio release follow-up

The current source tree no longer tracks the legacy `build/chip8.exe`, generated SPIR-V files, third-party ROMs or unused CppWinRT package manifest. Their local copies were retained, and Git history was not rewritten. Generated shaders are built from GLSL source.

The v0.1.0 Windows x64 preview uses the static Microsoft C++ runtime. Its package script includes only the executable, two shaders, instructions and dependency notices, plus a separate SHA-256 checksum. No ROMs or build intermediates are included. Windows CI repeats the Release build, core regression checks and package generation; a hosted runner does not verify Vulkan presentation or audible sound.
