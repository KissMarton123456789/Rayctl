#include "Engine.hpp"
#include <iostream>

int main()
{
    Engine engine(800, 600);

    if(!engine.init())
    {
        std::cerr <<"Engine initialization failed!\n";
        return 1; 
    }

    engine.run();

    return 0;
}