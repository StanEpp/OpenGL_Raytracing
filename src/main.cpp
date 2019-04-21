#include <iostream>

#include "Raytracer.hpp"

int main(int argc, char* argv[]){
    try
    {
        Raytracer raytracer("settings.txt", {argv + 1, argv + argc});
        raytracer.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
