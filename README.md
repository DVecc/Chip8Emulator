# C++ graphics and CHIP-8 projects

**Looking for the emulator? Open the [Chip8-Rendering branch](https://github.com/DVecc/Chip8Emulator/tree/Chip8-Rendering).**

The default `main` branch contains an earlier Pong game using a custom Vulkan renderer
and Win32 window/input code. The CHIP-8 interpreter was developed separately on
`Chip8-Rendering`, which has its own Visual Studio project and emulator source.

| Project | Source |
| --- | --- |
| CHIP-8 interpreter / Vulkan display | [Emulator branch](https://github.com/DVecc/Chip8Emulator/tree/Chip8-Rendering) |
| Earlier Pong game | [game.cpp](src/Game/game.cpp) |
| Earlier Vulkan renderer | [vk_renderer.cpp](src/renderer/vk_renderer.cpp) |

The branches have different layouts and build setups. Use the branch-specific instructions;
the default branch's `build.bat` is not the emulator build command.

Generated application binaries, debug symbols and object files are excluded from this
proposed source tree. Existing history is preserved. A tested binary release and recorded
demo remain follow-up work on a Windows/Vulkan machine.

Documentation prepared with AI assistance and checked against repository contents.
