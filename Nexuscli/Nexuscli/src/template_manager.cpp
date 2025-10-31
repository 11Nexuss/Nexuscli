#include "template_manager.h"
#include <iostream>

TemplateManager::TemplateManager() {
    LoadAllTemplates();
}

bool TemplateManager::TemplateExists(const std::string& templateName) {
    return templates.find(templateName) != templates.end();
}

std::vector<std::string> TemplateManager::GetAvailableTemplates() {
    std::vector<std::string> availableTemplates;
    for (const auto& [name, _] : templates) {
        availableTemplates.push_back(name);
    }
    return availableTemplates;
}

std::map<std::string, std::string> TemplateManager::GetTemplateFiles(const std::string& templateName) {
    if (templates.find(templateName) != templates.end()) {
        return templates[templateName];
    }
    return {};
}

void TemplateManager::LoadAllTemplates() {
    LoadConsoleTemplate();
    LoadMemoryReaderTemplate();
}