//
// Created by plaidmrdeer on 2024/9/18.
//

#include <VulkanBase.h>

VulkanBase::VulkanBase() {

}

VulkanBase::~VulkanBase() {
    vkDestroyInstance(instance, nullptr);
}
