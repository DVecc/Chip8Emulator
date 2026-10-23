// C++ libraries
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>


// Vulkan Libraries
#include "renderer/vk_renderer.h"

// CHIP8 System
#include "emulator/chip8Emulator.h"

// Platform libraries
#ifdef WINDOWS_BUILD
#include "windows_platform/win32_platform.h"
#include <windows.h>
#include <wtypes.h>
#endif

// Utilities
#include "utils/logger.h"
#include <chrono>
#include <filesystem>

// CONSTANTS
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

// Global / External variables
bool running = false;
bool framebufferResized = false;




int main(int argc, char** argv)
{
    try
    {
        if (argc != 2) { std::cerr << "Usage: Chip8Emulator.exe <ROM path>\n"; return EXIT_FAILURE; }
        const auto romPath = std::filesystem::absolute(argv[1]);
        std::filesystem::current_path(std::filesystem::absolute(argv[0]).parent_path());
        Chip8Emulator emulator = {};
        VK_Renderer renderer = {};
        running = true;

        if (!platform_create_window(&emulator.window, WIDTH, HEIGHT))
        {
            LOG_FATAL(L"Failed to create window");
        }
        emulator.LoadROM(romPath.string().c_str());
        renderer.initVulkan(emulator.window);
        emulator.Chip8Init();
        platform_setInput(&emulator);
        auto lastCycleTime = std::chrono::steady_clock::now();

        while (running)
        {
            platform_update_window(emulator.window);

            uint32_t windowWidth, windowHeight;
            platform_get_window_size(emulator.window, &windowWidth, &windowHeight);
            if (!windowWidth || !windowHeight) { Sleep(10); continue; }
            Sleep(1);
            auto currentTime = std::chrono::steady_clock::now();
		    float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

            if (dt > 1000.0f / 60.0f)
            {
                if (running)
                {
                    lastCycleTime = currentTime;
                     for (int i = 0; i < 10; ++i)
                      {
                        emulator.cycle();
                      }
                    
                    emulator.tickTimers();
                    renderer.drawFrame(emulator.window, &framebufferResized, emulator.gfx);
                }
            }
            
        }
        vkDeviceWaitIdle(renderer.device);
        // Cleanup 
        renderer.cleanup();
        emulator.ShutdownSound();

        

    } catch ( const std::exception& error)
    {
        std::cerr << error.what() << std::endl; return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
