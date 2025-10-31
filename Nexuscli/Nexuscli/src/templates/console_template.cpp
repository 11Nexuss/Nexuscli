#include "../template_manager.h"

void TemplateManager::LoadConsoleTemplate() {
    templates["console"] = {
        {"src/main.cpp", R"(#include <iostream>

int main() {
    std::cout << "Hello from {{PROJECT_NAME}}!" << std::endl;
    std::cout << "Project generated with NexusCLI" << std::endl;
    
    std::cout << "Press Enter to exit...";
    std::cin.get();
    
    return 0;
})"}
    };
}