//
// Created by plaidmrdeer on 2024/9/18.
//

#ifndef VULKANBASE_H
#define VULKANBASE_H

#include <vulkan/vulkan.h>

class VulkanBase
{
public:
    VulkanBase();
    ~VulkanBase();
private:
    VkInstance instance;
};



#endif //VULKANBASE_H
