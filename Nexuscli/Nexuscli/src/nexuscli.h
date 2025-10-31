#pragma once
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class NexusCLI {
private:
    std::string projectName;
    std::string projectType;
    fs::path projectPath;
    std::string currentVersion = "1.0.0";

public:
    NexusCLI(int argc, char* argv[]);

    void ShowHelp();
    void ListTemplates();
    void CheckForUpdates();
    void CreateProject();

private:
    bool ValidateProjectName();
};