// C++ libraries
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>


// Vulkan Libraries
#include "renderer/vk_renderer.h"

// Platform libraries
#ifdef WINDOWS_BUILD
#include "windows_platform/win32_platform.h"
#include <windows.h>
#include <wtypes.h>
#endif

// Utilities
#include "utils/logger.h"

// CONSTANTS
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

// Global / External variables
bool running = false;
bool framebufferResized = false;

class Chip8Emulator
{
    public:
    #ifdef WINDOWS_BUILD
        HWND window;
    #endif

private:
    uint8_t gfx[64 * 32];

public:
    Chip8Emulator()
    {
        for (int i = 0; i < sizeof(gfx); i++)
        {
            gfx[i] = i % 2;
        }
    }

    void run(VK_Renderer renderer) 
    {
        while (running)
        {
            platform_update_window(window);
            if (running)
            {
                renderer.drawFrame(window, &framebufferResized, gfx);
            }
            
        }
        vkDeviceWaitIdle(renderer.device);
        // Cleanup 
        renderer.cleanup();
    }

    
};


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
        emulator.run(renderer);

        

    } catch ( const std::exception) 
    {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}


