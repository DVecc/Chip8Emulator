#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include <Windows.h>
#include "platform.h"
#include "defines.h"
#include "dds_structs.h"
#include "vk_types.h"
#include "vk_init.cpp"
#include "logger.h"
#include "vk_util.cpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
    VkDebugUtilsMessageTypeFlagsEXT msgFlags,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    ASSERT(0, pCallbackData->pMessage);
    return false;
}

struct VkContext
{

    VkDescriptorSet descSet;
    VkExtent2D screenSize;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice gpu;
    VkDevice device;
    VkQueue graphicsQueue;
    VkSwapchainKHR swapchain;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceFormatKHR surfaceFormat;
    VkCommandPool commandPool;
    VkSemaphore submitSemaphore;
    VkSemaphore acquireSemaphore;
    VkRenderPass renderPass;
    VkPipeline pipeline;
    VkPipelineLayout layout;
    Image image;
    Buffer StagingBuffer;
    // TODO: Suballocation from Main Memory
    VkImage scImages[5];
    VkImageView scImageViews[5];
    VkFramebuffer framebuffers[5];
    VkDescriptorSetLayout setLayout;
    VkDescriptorPool descPool;
    VkSampler sampler;

    uint32_t scImgCount;
    int graphicsIdx;
};

bool vk_init(VkContext *vkcontext, void *window)
{
    platform_get_window_size(&vkcontext->screenSize.width, &vkcontext->screenSize.height);

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Chip8Emulator";
    appInfo.pEngineName = "Chip8";

    char *extensions[] = {
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

    char *layers[] = {
        "VK_LAYER_KHRONOS_validation"};

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.ppEnabledExtensionNames = extensions;
    instanceInfo.enabledExtensionCount = ArraySize(extensions);
    instanceInfo.ppEnabledLayerNames = layers;
    instanceInfo.enabledLayerCount = ArraySize(layers);

    VK_CHECK(vkCreateInstance(&instanceInfo, 0, &vkcontext->instance));

    auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkcontext->instance, "vkCreateDebugUtilsMessengerEXT");

    if (vkCreateDebugUtilsMessengerEXT)
    {
        VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debugInfo.pfnUserCallback = vk_debug_callback;

        vkCreateDebugUtilsMessengerEXT(vkcontext->instance, &debugInfo, 0, &vkcontext->debugMessenger);
    }
    else
    {
        return false;
    }

    // Create Surface
    {
        VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.hwnd = (HWND)window;
        surfaceInfo.hinstance = GetModuleHandleA(0);

        VK_CHECK(vkCreateWin32SurfaceKHR(vkcontext->instance, &surfaceInfo, 0, &vkcontext->surface));
    }

    // Choose GPU
    {
        vkcontext->graphicsIdx = -1;
        uint32_t gpuCount = 0;
        // TODO: Suballocation from Main Allocation
        VkPhysicalDevice gpus[10];

        VK_CHECK(vkEnumeratePhysicalDevices(vkcontext->instance, &gpuCount, 0));
        VK_CHECK(vkEnumeratePhysicalDevices(vkcontext->instance, &gpuCount, gpus));

        for (uint32_t i = 0; i < gpuCount; i++)
        {
            VkPhysicalDevice gpu = gpus[i];

            uint32_t queueFamilyCount = 0;
            // TODO: Suballocation from Main Allocation
            VkQueueFamilyProperties queueProps[10];

            vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, 0);
            vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueProps);

            for (uint32_t j = 0; j < queueFamilyCount; j++)
            {
                VkBool32 surfaceSupport = VK_FALSE;
                if (queueProps[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, j, vkcontext->surface, &surfaceSupport));

                    if (surfaceSupport)
                    {
                        vkcontext->graphicsIdx = j;
                        vkcontext->gpu = gpu;
                        break;
                    }
                }
            }
        }

        if (vkcontext->graphicsIdx < 0)
        {
            return false;
        }
    }

    // Logical Device
    {
        float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo queueInfo = {};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = vkcontext->graphicsIdx;
        queueInfo.pQueuePriorities = &queuePriority;
        queueInfo.queueCount = 1;

        char *extensions[] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkDeviceCreateInfo deviceInfo = {};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.pQueueCreateInfos = &queueInfo;
        deviceInfo.queueCreateInfoCount = 1;
        deviceInfo.ppEnabledExtensionNames = extensions;
        deviceInfo.enabledExtensionCount = ArraySize(extensions);
        VK_CHECK(vkCreateDevice(vkcontext->gpu, &deviceInfo, 0, &vkcontext->device));
        vkGetDeviceQueue(vkcontext->device, vkcontext->graphicsIdx, 0, &vkcontext->graphicsQueue);
    }

    // Swapchain
    {
        uint32_t formatCount = 0;
        // TODO: Suballocation from Main Memory
        VkSurfaceFormatKHR surfaceFormats[10];
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vkcontext->gpu, vkcontext->surface, &formatCount, 0));
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vkcontext->gpu, vkcontext->surface, &formatCount, surfaceFormats));

        for (uint32_t i = 0; i < formatCount; i++)
        {
            VkSurfaceFormatKHR format = surfaceFormats[i];

            if (format.format == VK_FORMAT_B8G8R8A8_SRGB)
            {
                vkcontext->surfaceFormat = format;
                break;
            }
        }

        VkSurfaceCapabilitiesKHR surfaceCaps = {};
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkcontext->gpu, vkcontext->surface, &surfaceCaps));
        uint32_t imgCount = surfaceCaps.minImageCount + 1;
        imgCount = imgCount > surfaceCaps.maxImageCount ? imgCount - 1 : imgCount;

        VkSwapchainCreateInfoKHR scInfo = {};
        scInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        scInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        scInfo.surface = vkcontext->surface;
        scInfo.preTransform = surfaceCaps.currentTransform;
        scInfo.imageExtent = surfaceCaps.currentExtent;
        scInfo.minImageCount = imgCount;
        scInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        scInfo.imageArrayLayers = 1;
        scInfo.imageFormat = vkcontext->surfaceFormat.format;
        VK_CHECK(vkCreateSwapchainKHR(vkcontext->device, &scInfo, 0, &vkcontext->swapchain));

        VK_CHECK(vkGetSwapchainImagesKHR(vkcontext->device, vkcontext->swapchain, &vkcontext->scImgCount, 0));
        VK_CHECK(vkGetSwapchainImagesKHR(vkcontext->device, vkcontext->swapchain, &vkcontext->scImgCount, vkcontext->scImages));

        // Create the image views
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.format = vkcontext->surfaceFormat.format;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.levelCount = 1;

        for (uint32_t i = 0; i < vkcontext->scImgCount; i++)
        {
            viewInfo.image = vkcontext->scImages[i];
            VK_CHECK(vkCreateImageView(vkcontext->device, &viewInfo, 0, &vkcontext->scImageViews[i]));
        }
    }

    // Render Pass
    {
        VkAttachmentDescription attachment = {};
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.format = vkcontext->surfaceFormat.format;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDesc = {};
        subpassDesc.colorAttachmentCount = 1;
        subpassDesc.pColorAttachments = &colorAttachmentRef;

        VkAttachmentDescription attachments[] = {
            attachment};

        VkRenderPassCreateInfo rpinfo = {};
        rpinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpinfo.pAttachments = attachments;
        rpinfo.attachmentCount = ArraySize(attachments);
        rpinfo.subpassCount = 1;
        rpinfo.pSubpasses = &subpassDesc;
        VK_CHECK(vkCreateRenderPass(vkcontext->device, &rpinfo, 0, &vkcontext->renderPass));
    }

    // Framebuffer
    {
        VkFramebufferCreateInfo fbinfo = {};
        fbinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbinfo.height = vkcontext->screenSize.height;
        fbinfo.width = vkcontext->screenSize.width;
        fbinfo.renderPass = vkcontext->renderPass;
        fbinfo.layers = 1;
        fbinfo.attachmentCount = 1;

        for (uint32_t i = 0; i < vkcontext->scImgCount; i++)
        {
            fbinfo.pAttachments = &vkcontext->scImageViews[i];
            VK_CHECK(vkCreateFramebuffer(vkcontext->device, &fbinfo, 0, &vkcontext->framebuffers[i]));
        }
    }

    // Descriptor Set Layout
    {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = 0;
        binding.descriptorCount = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &binding;
        VK_CHECK(vkCreateDescriptorSetLayout(vkcontext->device, &layoutInfo, 0, &vkcontext->setLayout));
    }

    // Pipeline Layout
    {
        VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = 1;
        layoutInfo.pSetLayouts = &vkcontext->setLayout;
        VK_CHECK(vkCreatePipelineLayout(vkcontext->device, &layoutInfo, 0, &vkcontext->layout));
    }

    // Pipeline
    {
        VkShaderModule vertexShader, fragmentShader;
        uint32_t sizeInBytes;
        char *code = platform_read_file("assets/shaders/shader.vert.spv", &sizeInBytes);

        VkShaderModuleCreateInfo shaderInfo = {};
        shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderInfo.pCode = (uint32_t *)code;
        shaderInfo.codeSize = sizeInBytes;
        VK_CHECK(vkCreateShaderModule(vkcontext->device, &shaderInfo, 0, &vertexShader));
        // TODO: Suballocation from main allocation
        delete code;

        code = platform_read_file("assets/shaders/shader.frag.spv", &sizeInBytes);
        shaderInfo.pCode = (uint32_t *)code;
        shaderInfo.codeSize = sizeInBytes;
        VK_CHECK(vkCreateShaderModule(vkcontext->device, &shaderInfo, 0, &fragmentShader));
        // TODO: Suballocation from main allocation
        delete code;

        VkPipelineShaderStageCreateInfo vertexStage = {};
        vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexStage.pName = "main";
        vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexStage.module = vertexShader;

        VkPipelineShaderStageCreateInfo fragmentStage = {};
        fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentStage.pName = "main";
        fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentStage.module = fragmentShader;

        VkPipelineShaderStageCreateInfo shaderStages[] = {
            vertexStage,
            fragmentStage};

        VkPipelineVertexInputStateCreateInfo vertexInputState = {};
        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VkPipelineColorBlendAttachmentState colorAttachment = {};
        colorAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlendState = {};
        colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendState.pAttachments = &colorAttachment;
        colorBlendState.attachmentCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizationState = {};
        rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationState.lineWidth = 1.0f;

        VkRect2D scissor = {};
        VkViewport viewport = {};

        VkPipelineMultisampleStateCreateInfo multisampleState = {};
        multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkPipelineViewportStateCreateInfo viewPortState = {};
        viewPortState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewPortState.scissorCount = 1;
        viewPortState.viewportCount = 1;
        viewPortState.pScissors = &scissor;
        viewPortState.pViewports = &viewport;

        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_VIEWPORT};

        VkPipelineDynamicStateCreateInfo dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.pDynamicStates = dynamicStates;
        dynamicState.dynamicStateCount = ArraySize(dynamicStates);

        VkGraphicsPipelineCreateInfo pipeInfo = {};
        pipeInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeInfo.pVertexInputState = &vertexInputState;
        pipeInfo.pColorBlendState = &colorBlendState;
        pipeInfo.pStages = shaderStages;
        pipeInfo.stageCount = ArraySize(shaderStages);
        pipeInfo.pRasterizationState = &rasterizationState;
        pipeInfo.layout = vkcontext->layout;
        pipeInfo.renderPass = vkcontext->renderPass;
        pipeInfo.pViewportState = &viewPortState;
        pipeInfo.pDynamicState = &dynamicState;
        pipeInfo.pMultisampleState = &multisampleState;
        pipeInfo.pInputAssemblyState = &inputAssembly;

        VK_CHECK(vkCreateGraphicsPipelines(vkcontext->device, 0, 1, &pipeInfo, 0, &vkcontext->pipeline));
        vkDestroyShaderModule(vkcontext->device, vertexShader, 0);
        vkDestroyShaderModule(vkcontext->device, fragmentShader, 0);
    }

    // Staging Buffer
    {
        vkcontext->StagingBuffer = vk_allocate_buffer(vkcontext->device, vkcontext->gpu, MB(1), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
      
    }

    // Command Pool
    {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = vkcontext->graphicsIdx;
        VK_CHECK(vkCreateCommandPool(vkcontext->device, &poolInfo, 0, &vkcontext->commandPool));
    }

    // Create Image
    {
        uint32_t fileSize;
        DDSFile *data = (DDSFile *)platform_read_file("assets/textures/cakez.DDS", &fileSize);
        uint32_t textureSize = data->header.Width * data->header.Height * 4;
        vk_copy_to_buffer(&vkcontext->StagingBuffer, &data->dataBegin, textureSize);

        // TODO: Assertions
        vkcontext->image = vk_allocate_image(vkcontext->device, vkcontext->gpu, data->header.Width, data->header.Height, VK_FORMAT_R8G8B8A8_UNORM);

        VkCommandBuffer cmd;
        VkCommandBufferAllocateInfo cmdAlloc = cmd_alloc_info(vkcontext->commandPool);
        vkAllocateCommandBuffers(vkcontext->device, &cmdAlloc, &cmd);
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

        VkImageSubresourceRange range = {};
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.layerCount = 1;
        range.levelCount = 1;

        // transition layout to transfer optimal
        VkImageMemoryBarrier imgMemBarrier = {};
        imgMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imgMemBarrier.image = vkcontext->image.image;
        imgMemBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imgMemBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imgMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        imgMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imgMemBarrier.subresourceRange = range;
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &imgMemBarrier);

        VkBufferImageCopy copyRegion = {};
        copyRegion.imageExtent = {data->header.Width, data->header.Height, 1};
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        vkCmdCopyBufferToImage(cmd, vkcontext->StagingBuffer.buffer, vkcontext->image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        imgMemBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imgMemBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        imgMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &imgMemBarrier);

        VK_CHECK(vkEndCommandBuffer(cmd));

        VkFence uploadFence;
        VkFenceCreateInfo fenceInfo = fence_info();
        VK_CHECK(vkCreateFence(vkcontext->device, &fenceInfo, 0, &uploadFence));

        VkSubmitInfo submitInfo = submit_info(&cmd);
        VK_CHECK(vkQueueSubmit(vkcontext->graphicsQueue, 1, &submitInfo, uploadFence));

        VK_CHECK(vkWaitForFences(vkcontext->device, 1, &uploadFence, true, UINT64_MAX));
    }

    
    // Sync Objects
    {
        VkSemaphoreCreateInfo semaInfo = {};
        semaInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VK_CHECK(vkCreateSemaphore(vkcontext->device, &semaInfo, 0, &vkcontext->acquireSemaphore));
        VK_CHECK(vkCreateSemaphore(vkcontext->device, &semaInfo, 0, &vkcontext->submitSemaphore));

        // VkFenceCreateInfo fenceInfo = fence_info(VK_FENCE_CREATE_SIGNALED_BIT);
        // VK_CHECK(vkCreateFence(vkcontext->device, &fenceInfo, 0, &vkcontext->imgAvailableFence));
    }

    // create image view
    {
        VkImageViewCreateInfo imgViewInfo = {};
        imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imgViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgViewInfo.image = vkcontext->image.image;
        imgViewInfo.subresourceRange.layerCount = 1;
        imgViewInfo.subresourceRange.levelCount = 1;
        imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        vkCreateImageView(vkcontext->device, &imgViewInfo, 0, &vkcontext->image.view);
    }

    // Create Sampler
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.minFilter = VK_FILTER_NEAREST;
        samplerInfo.magFilter = VK_FILTER_NEAREST;

        VK_CHECK(vkCreateSampler(vkcontext->device, &samplerInfo, 0, &vkcontext->sampler));
    }

    // Create Descriptor Pool
    {
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.maxSets = 1;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        VK_CHECK(vkCreateDescriptorPool(vkcontext->device, &poolInfo, 0, &vkcontext->descPool))
    }

    //  Create Descriptor Set
    {
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &vkcontext->setLayout;
        allocInfo.descriptorPool = vkcontext->descPool;
        VK_CHECK(vkAllocateDescriptorSets(vkcontext->device, &allocInfo, &vkcontext->descSet));
    }

    // Update Descriptor Set
    {
        VkDescriptorImageInfo imgInfo = {};
        imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgInfo.imageView = vkcontext->image.view;
        imgInfo.sampler = vkcontext->sampler;

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = vkcontext->descSet;
        write.pImageInfo = &imgInfo;
        write.dstBinding = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        vkUpdateDescriptorSets(vkcontext->device, 1, &write, 0, 0);
    }

    return true;
}

bool vk_render(VkContext *vkcontext)
{
    uint32_t imgIdx;
    VK_CHECK(vkAcquireNextImageKHR(vkcontext->device, vkcontext->swapchain, 0, vkcontext->acquireSemaphore, 0, &imgIdx));

    VkCommandBuffer cmd;

    VkCommandBufferAllocateInfo info = cmd_alloc_info(vkcontext->commandPool);
    VK_CHECK(vkAllocateCommandBuffers(vkcontext->device, &info, &cmd));

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

    VkClearValue clearValue = {};
    clearValue.color = {1, 1, 0, 1};

    VkRenderPassBeginInfo rpBeginInfo = {};
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = vkcontext->renderPass;
    rpBeginInfo.renderArea.extent = vkcontext->screenSize;
    rpBeginInfo.framebuffer = vkcontext->framebuffers[imgIdx];
    rpBeginInfo.pClearValues = &clearValue;
    rpBeginInfo.clearValueCount = 1;

    vkCmdBeginRenderPass(cmd, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Rendering Commands
    {
        VkRect2D scissor = {};
        scissor.extent = vkcontext->screenSize;

        VkViewport viewport = {};
        viewport.height = vkcontext->screenSize.height;
        viewport.width = vkcontext->screenSize.width;
        viewport.maxDepth = 1.0f;

        vkCmdSetScissor(cmd, 0, 1, &scissor);
        vkCmdSetViewport(cmd, 0, 1, &viewport);

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkcontext->layout, 0, 1, &vkcontext->descSet, 0, 0);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkcontext->pipeline);
        vkCmdDraw(cmd, 6, 1, 0, 0);
    }

    vkCmdEndRenderPass(cmd);

    VK_CHECK(vkEndCommandBuffer(cmd));

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = submit_info(&cmd);
    submitInfo.pSignalSemaphores = &vkcontext->submitSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &vkcontext->acquireSemaphore;
    submitInfo.pWaitDstStageMask = &waitStage;

    VK_CHECK(vkQueueSubmit(vkcontext->graphicsQueue, 1, &submitInfo, 0));

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pSwapchains = &vkcontext->swapchain;
    presentInfo.swapchainCount = 1;
    presentInfo.pImageIndices = &imgIdx;
    presentInfo.pWaitSemaphores = &vkcontext->submitSemaphore;
    presentInfo.waitSemaphoreCount = 1;

    VK_CHECK(vkQueuePresentKHR(vkcontext->graphicsQueue, &presentInfo));

    VK_CHECK(vkDeviceWaitIdle(vkcontext->device));

    vkFreeCommandBuffers(vkcontext->device, vkcontext->commandPool, 1, &cmd);
    return true;
}