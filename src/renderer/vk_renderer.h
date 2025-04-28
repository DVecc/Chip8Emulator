#ifndef VK_RENDER_H
#define VK_RENDER_H

// Utilities
#include "../utils/logger.h"

// VKCHECK MACRO
#define VK_CHECK(result)                                       \
    if (result != VK_SUCCESS)                                  \
    {                                                          \
        LOG_ERROR(L"Vulkan: Error: %d", result);                \
        __debugbreak();                                        \
    }

// Vulkan Libraries
#include <vulkan/vulkan.h>

class VK_Renderer {
    public: 
        VkInstance instance;
        bool initVulkan();
       

    private: 
    
        void createInstance();
};

#endif