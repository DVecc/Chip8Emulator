#include "vk_renderer.h"

bool VK_Renderer::initVulkan()
{
    createInstance();

    return true;
};

void VK_Renderer::createInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "CHIP-8 Emulator";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = {};

    VK_CHECK(vkCreateInstance(&createInfo, 0, &this->instance));

}