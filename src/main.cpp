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

// CONSTANTS
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

// Global / External variables
bool running = false;
bool framebufferResized = false;




static void vk_compile_shader(char *shaderPath, char *sprivPath)
{
    ASSERT(shaderPath, L"No shader path supplied");
    ASSERT(sprivPath, L"No spriv path supplied");

    char command[320] = {};
    sprintf_s(command, sizeof(command), "C:\\VulkanSDK\\1.4.309.0\\Bin\\glslc.exe %s -o %s", shaderPath, sprivPath);

    int result = system(command);

    ASSERT(!result, L"Failed to compile shader: %s", shaderPath);
}

int main()
{
    try
    {
        #ifdef DEBUG
        vk_compile_shader("C:\\Users\\Vincent\\Desktop\\Chip8Emulator\\src\\shaders\\shader.vert", "C:\\Users\\Vincent\\Desktop\\Chip8Emulator\\src\\shaders\\vert.spv");
        vk_compile_shader("C:\\Users\\Vincent\\Desktop\\Chip8Emulator\\src\\shaders\\shader.frag", "C:\\Users\\Vincent\\Desktop\\Chip8Emulator\\src\\shaders\\frag.spv");
        #endif // DEBUG

        Chip8Emulator emulator = {};
        VK_Renderer renderer = {};
        running = true;

        if (!platform_create_window(&emulator.window, WIDTH, HEIGHT))
        {
            LOG_FATAL(L"Failed to create window");
        }
        renderer.initVulkan(emulator.window);
        emulator.Chip8Init();
        platform_setInput(&emulator);
        emulator.LoadROM("src/Rush Hour [Hap, 2006].ch8");
        auto lastCycleTime = std::chrono::high_resolution_clock::now();

        while (running)
        {
            platform_update_window(emulator.window);

            auto currentTime = std::chrono::high_resolution_clock::now();
		    float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

            if (dt > 1000 / 60)
            {
                if (running)
                {
                    lastCycleTime = currentTime;
                     for (int i = 0; i < 10; ++i)
                      {
                        emulator.cycle();
                      }
                    
                    renderer.drawFrame(emulator.window, &framebufferResized, emulator.gfx);
                }
            }
            
        }
        vkDeviceWaitIdle(renderer.device);
        // Cleanup 
        renderer.cleanup();

        

    } catch ( const std::exception) 
    {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}


