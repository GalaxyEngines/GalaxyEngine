//
// Created by plaidmrdeer on 2024/11/23.
//

#ifndef GALAXY_ENGINE_H
#define GALAXY_ENGINE_H

#include <application/galaxy_engine.h>
#include <application/logger.h>
#include <graphics/graphics.h>
#include <application/window.h>

namespace ge
{
    class GalaxyEngine
    {
    public:
        void init();
        void run() const;
    private:
        Logger *logger_ = nullptr;
        Window *window_ = nullptr;
        Graphics *graphics_ = nullptr;
    };
}

#endif
