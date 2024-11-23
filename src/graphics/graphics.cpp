//
// Created by plaidmrdeer on 2024/11/23.
//

#include <graphics/graphics.h>

#include <VkBootstrap.h>

ge::Graphics::~Graphics()
{
}

void ge::Graphics::init()
{
    init_vulkan();
}

void ge::Graphics::draw()
{
    loop();
}

void ge::Graphics::init_vulkan()
{
    vkb::InstanceBuilder instance_builder;
    auto instance_builder_return = instance_builder
            // Instance creation configuration
            .request_validation_layers()
            .use_default_debug_messenger()
            .build ();
    if (!instance_builder_return) {
        // Handle error
    }
    vkb::Instance vkb_instance = instance_builder_return.value ();

    vkb::destroy_instance(vkb_instance);
}

void ge::Graphics::loop()
{

}
