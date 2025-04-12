#pragma once
#include <vulkan/vulkan.h>
#include "logger.h"
#define VK_CHECK(result)                                       \
    if (result != VK_SUCCESS)                                  \
    {                                                          \
        LOG_ERROR("Vulkan: Error: %d", result);                \
        __debugbreak();                                        \
    }

    struct Image
{
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
};

struct Buffer{
    VkBuffer buffer;
    VkDeviceMemory memory;
    uint32_t size;
    void* data;
};

