// C++ libraries
#include <iostream>
#include <stdexcept>
#include <cstdlib>

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

class Chip8Emulator
{
    public:
    #ifdef WINDOWS_BUILD
        HWND window;
    #endif

    void run() 
    {
        while (running)
        {
            platform_update_window(window);
        }
    }
};

int main()
{

   
    try
    {
        Chip8Emulator emulator = {};
        VK_Renderer renderer = {};
        running = true;

        if (!platform_create_window(emulator.window, WIDTH, HEIGHT))
        {
            LOG_FATAL(L"Failed to create window");
        }
        LOG_FATAL(L"Failed to create window");
        renderer.initVulkan();
        emulator.run();

    } catch ( const std::exception) 
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
