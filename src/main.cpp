#include <iostream>

#include <application/galaxy_engine.h>

int main()
{
    ge::GalaxyEngine engine_{};
    engine_.init();
    engine_.run();

    std::cout << "所有模块已成功关闭，程序结束。" << std::endl;
    return 0;
}