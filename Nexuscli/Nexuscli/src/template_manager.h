#pragma once
#include <string>
#include <vector>
#include <map>
#include <filesystem>

namespace fs = std::filesystem;

class TemplateManager {
public:
    TemplateManager();

    bool TemplateExists(const std::string& templateName);
    std::vector<std::string> GetAvailableTemplates();
    std::map<std::string, std::string> GetTemplateFiles(const std::string& templateName);

private:
    void LoadAllTemplates();

    // Template storage
    std::map<std::string, std::map<std::string, std::string>> templates;

    // Individual template loaders
    void LoadConsoleTemplate();
    void LoadMemoryReaderTemplate();
};