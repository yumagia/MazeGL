#include <iostream>
#include "render/Application.hpp"


int main() {
    Application app;

    app.Init();

    std::cout << "RUNNING APPLICATION..." << std::endl;
    return app.Run();
}