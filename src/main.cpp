#include <iostream>
#include "render/Application.hpp"


int main() {
    Application app;
    
    std::cout << "INITIALIZING APPLICATION..." << std::endl;
    app.Init();

    std::cout << "RUNNING APPLICATION..." << std::endl;
    return app.Run();
}