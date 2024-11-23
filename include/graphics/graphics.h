//
// Created by plaidmrdeer on 2024/11/23.
//

#ifndef GRAPHICS_H
#define GRAPHICS_H

namespace ge
{
    class Graphics
    {
    public:
        ~Graphics();

        void init();
        void draw();
    private:
        void init_vulkan();
        void loop();
    };
}

#endif
