#include <vulkan/vulkan.h>
#include "vk_types.h"

VkCommandBufferAllocateInfo cmd_alloc_info(VkCommandPool pool)
{
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandBufferCount = 1;
    info.commandPool = pool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    return info;
}

VkFenceCreateInfo fence_info(VkFenceCreateFlags flags = 0) {
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.flags = flags;

    return info;
}

VkSubmitInfo submit_info(VkCommandBuffer* cmd, uint32_t cmdCount = 1) 
{
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = cmdCount;
    submitInfo.pCommandBuffers = cmd;

    return submitInfo;
    
}

VkDescriptorSetLayoutBinding layout_binding(
    VkDescriptorType type,
    VkShaderStageFlags shaderStages,
    uint32_t count,
    uint32_t bindingNumber)
{
    VkDescriptorSetLayoutBinding binding = {};
        binding.binding = bindingNumber;
        binding.descriptorCount = count;
        binding.descriptorType = type;
        binding.stageFlags = shaderStages;

        return binding;
}

VkWriteDescriptorSet write_set(
    VkDescriptorSet set,
    VkDescriptorType type,
    DescriptorInfo* descInfo,
    uint32_t bindingIdx,
    uint32_t count)
{
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set;
    write.pImageInfo = &descInfo->imageInfo;
    write.pBufferInfo = &descInfo->bufferInfo;
    write.dstBinding = bindingIdx;
    write.descriptorType = type;
    write.descriptorCount = count;
    
    return write;
}