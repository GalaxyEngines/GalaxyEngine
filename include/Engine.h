//
// Created by plaidmrdeer on 2024/9/14.
//

#ifndef ENGINE_H
#define ENGINE_H



class Engine
{
public:
    void run();
private:
    void init();
    void initVulkan();
    void initWindow();
    void loop();
    void cleanup();
};



#endif //ENGINE_H
