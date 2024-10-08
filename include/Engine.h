//
// Created by plaidmrdeer on 2024/9/14.
//

#pragma once
#ifndef ENGINE_H
#define ENGINE_H

class Engine
{

public:
    void run();
private:
    GLFWwindow* window = nullptr;
    const int WIDTH = 1920;
    const int HEIGHT = 1080;

    void init();
    void initVulkan();
    void initWindow();
    void loop();
    void cleanup();
};



#endif //ENGINE_H
