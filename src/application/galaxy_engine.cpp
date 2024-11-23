//
// Created by plaidmrdeer on 2024/11/23.
//

#include <application/galaxy_engine.h>

void ge::GalaxyEngine::init()
{
    logger_ = new Logger("logs");
    logger_->log(INFO, "Initializing Galaxy engine...");

    window_ = new Window();
    window_->create_window(1920, 1080, "Galaxy Engine");
}

void ge::GalaxyEngine::run() const
{
    logger_->log(INFO, "Running Galaxy engine...");

    while (!glfwWindowShouldClose(window_->get_window()))
    {
        glfwPollEvents();
    }
}

