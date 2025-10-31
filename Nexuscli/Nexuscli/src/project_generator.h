#pragma once
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class ProjectGenerator {
private:
    std::string projectName;
    std::string projectType;
    fs::path projectPath;

public:
    ProjectGenerator(const std::string& name, const std::string& type, const fs::path& path);

    void GenerateProject();

private:
    void CreateProjectStructure();
    void GenerateSourceFiles();
    void GenerateVisualStudioFiles();
    void GenerateSolutionFile();
    void GenerateProjectFile();
    void GenerateFiltersFile();
    void GenerateReadme();
    std::string GenerateGUID();
    void ReplaceTemplateVariables(std::string& content);
};