#include "nexuscli.h"
#include "template_manager.h"
#include "project_generator.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

NexusCLI::NexusCLI(int argc, char* argv[]) {
    if (argc < 2) {
        ShowHelp();
        return;
    }

    std::string command = argv[1];

    if (command == "new") {
        if (argc < 3) {
            std::cout << "Error: Project name required" << std::endl;
            ShowHelp();
            return;
        }
        projectName = argv[2];
        projectType = (argc > 3) ? argv[3] : "console";
        projectPath = fs::current_path() / projectName;
        CreateProject();
    }
    else if (command == "update") {
        CheckForUpdates();
    }
    else if (command == "version") {
        std::cout << "NexusCLI Version: " << currentVersion << std::endl;
    }
    else if (command == "list") {
        ListTemplates();
    }
    else if (command == "help") {
        ShowHelp();
    }
    else {
        std::cout << "Unknown command: " << command << std::endl;
        ShowHelp();
    }
}

void NexusCLI::ShowHelp() {
    std::cout << "NexusCLI - C++ Project Generator" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Version: " << currentVersion << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  nexus new <project_name> [type]" << std::endl;
    std::cout << "  nexus update                    Check for updates" << std::endl;
    std::cout << "  nexus version                   Show version" << std::endl;
    std::cout << "  nexus list                      List templates" << std::endl;
    std::cout << "  nexus help                      Show this help" << std::endl;
    std::cout << std::endl;

    TemplateManager tm;
    auto templates = tm.GetAvailableTemplates();

    std::cout << "Project Types:" << std::endl;
    for (const auto& name : templates) {
        std::cout << "  " << name;
        if (name == "memory-reader") std::cout << " - Game memory reading application";
        else if (name == "console") std::cout << "    - Basic console application";
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nexus new MyGameHack memory-reader" << std::endl;
    std::cout << "  nexus new MyTool console" << std::endl;
    std::cout << std::endl;
    std::cout << "More templates and updates:" << std::endl;
    std::cout << "  https://github.com/11Nexuss/NexusCLI" << std::endl;
}

void NexusCLI::ListTemplates() {
    TemplateManager tm;
    auto templates = tm.GetAvailableTemplates();

    std::cout << "Available templates:" << std::endl;
    for (const auto& name : templates) {
        std::cout << "  - " << name << std::endl;
    }
}

void NexusCLI::CheckForUpdates() {
    std::cout << "Checking for updates..." << std::endl;
    std::cout << "You have the latest version: " << currentVersion << std::endl;
    std::cout << std::endl;
    std::cout << "To update manually:" << std::endl;
    std::cout << "1. Visit: https://github.com/11Nexuss/NexusCLI" << std::endl;
    std::cout << "2. Download the latest release" << std::endl;
    std::cout << "3. Run install.bat again" << std::endl;
}

void NexusCLI::CreateProject() {
    if (fs::exists(projectPath)) {
        std::cout << "Error: Project directory already exists!" << std::endl;
        return;
    }

    TemplateManager tm;
    if (!tm.TemplateExists(projectType)) {
        std::cout << "Error: Unknown project type '" << projectType << "'" << std::endl;
        std::cout << "Use 'nexus list' to see available templates." << std::endl;
        return;
    }

    std::cout << "Creating " << projectType << " project: " << projectName << std::endl;

    try {
        ProjectGenerator generator(projectName, projectType, projectPath);
        generator.GenerateProject();

        std::cout << std::endl;
        std::cout << "✅ Project created successfully!" << std::endl;
        std::cout << "📍 Location: " << projectPath << std::endl;
        std::cout << std::endl;
        std::cout << "Next steps:" << std::endl;
        std::cout << "1. Open " << projectName << ".sln in Visual Studio" << std::endl;
        std::cout << "2. Build with Ctrl+Shift+B" << std::endl;
        std::cout << "3. Run with F5" << std::endl;
        std::cout << std::endl;
        std::cout << "Need more templates or advanced features?" << std::endl;
        std::cout << "Visit: https://github.com/11Nexuss/NexusCLI-Templates" << std::endl;

    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}