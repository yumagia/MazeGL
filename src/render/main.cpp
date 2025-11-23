#include <iostream>
#include "render/Application.hpp"


int main() {
    std::cout << "RUNNING APPLICATION..." << std::endl;

    Application app;

    return app.Run();
}