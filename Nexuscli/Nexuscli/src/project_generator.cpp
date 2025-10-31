#include "project_generator.h"
#include "template_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

namespace fs = std::filesystem;

ProjectGenerator::ProjectGenerator(const std::string& name, const std::string& type, const fs::path& path)
    : projectName(name), projectType(type), projectPath(path) {
}

void ProjectGenerator::GenerateProject() {
    std::cout << "Creating project structure..." << std::endl;

    CreateProjectStructure();
    GenerateSourceFiles();
    GenerateVisualStudioFiles();
}

void ProjectGenerator::CreateProjectStructure() {
    fs::create_directories(projectPath);
    fs::create_directories(projectPath / "src");
    fs::create_directories(projectPath / "include");
    fs::create_directories(projectPath / "lib");

    std::cout << "  Created: src/" << std::endl;
    std::cout << "  Created: include/" << std::endl;
    std::cout << "  Created: lib/" << std::endl;
}

void ProjectGenerator::GenerateSourceFiles() {
    std::cout << "Generating source files..." << std::endl;

    TemplateManager tm;
    auto templateFiles = tm.GetTemplateFiles(projectType);

    for (const auto& [filePath, content] : templateFiles) {
        fs::path fullPath = projectPath / filePath;
        fs::create_directories(fullPath.parent_path());

        std::ofstream file(fullPath);
        std::string processedContent = content;
        ReplaceTemplateVariables(processedContent);
        file << processedContent;

        std::cout << "  Created: " << filePath << std::endl;
    }
}

void ProjectGenerator::ReplaceTemplateVariables(std::string& content) {
    size_t pos = 0;
    while ((pos = content.find("{{PROJECT_NAME}}", pos)) != std::string::npos) {
        content.replace(pos, 16, projectName);
        pos += projectName.length();
    }
}

std::string ProjectGenerator::GenerateGUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    ss << std::hex;
    ss << "{";
    for (int i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-";
    ss << dis2(gen);
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dis(gen);
    ss << "}";

    return ss.str();
}

void ProjectGenerator::GenerateVisualStudioFiles() {
    std::cout << "Generating Visual Studio project files..." << std::endl;

    GenerateSolutionFile();
    GenerateProjectFile();
    GenerateFiltersFile();
    GenerateReadme();

    std::cout << "  Created: " << projectName << ".sln" << std::endl;
    std::cout << "  Created: " << projectName << ".vcxproj" << std::endl;
    std::cout << "  Created: " << projectName << ".vcxproj.filters" << std::endl;
    std::cout << "  Created: README.md" << std::endl;
}

void ProjectGenerator::GenerateSolutionFile() {
    std::string projectGUID = GenerateGUID();

    std::ofstream slnFile(projectPath / (projectName + ".sln"));

    slnFile << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
    slnFile << "# Visual Studio Version 17\n";
    slnFile << "VisualStudioVersion = 17.0.31903.59\n";
    slnFile << "MinimumVisualStudioVersion = 10.0.40219.1\n";
    slnFile << "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"" << projectName << "\", \"" << projectName << ".vcxproj\", \"" << projectGUID << "\"\n";
    slnFile << "EndProject\n";
    slnFile << "Global\n";
    slnFile << "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
    slnFile << "\t\tDebug|x64 = Debug|x64\n";
    slnFile << "\t\tRelease|x64 = Release|x64\n";
    slnFile << "\tEndGlobalSection\n";
    slnFile << "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n";
    slnFile << "\t\t" << projectGUID << ".Debug|x64.ActiveCfg = Debug|x64\n";
    slnFile << "\t\t" << projectGUID << ".Debug|x64.Build.0 = Debug|x64\n";
    slnFile << "\t\t" << projectGUID << ".Release|x64.ActiveCfg = Release|x64\n";
    slnFile << "\t\t" << projectGUID << ".Release|x64.Build.0 = Release|x64\n";
    slnFile << "\tEndGlobalSection\n";
    slnFile << "\tGlobalSection(SolutionProperties) = preSolution\n";
    slnFile << "\t\tHideSolutionNode = FALSE\n";
    slnFile << "\tEndGlobalSection\n";
    slnFile << "EndGlobal\n";
}

void ProjectGenerator::GenerateProjectFile() {
    std::ofstream vcxprojFile(projectPath / (projectName + ".vcxproj"));

    vcxprojFile << R"(<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>)" << GenerateGUID() << R"(</ProjectGuid>
    <Keyword>Win32Proj</Keyword>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
    <ProjectName>)" << projectName << R"(</ProjectName>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v143</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v143</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <PropertyGroup>
    <IncludePath>include;$(IncludePath)</IncludePath>
  </PropertyGroup>
  <ItemGroup>
    <ClCompile Include="src\main.cpp" />)" << "\n";

    if (projectType == "memory-reader") {
        vcxprojFile << "    <ClCompile Include=\"src\\memory.cpp\" />\n";
    }

    vcxprojFile << R"(  </ItemGroup>
  <ItemGroup>)" << "\n";

    if (projectType == "memory-reader") {
        vcxprojFile << "    <ClInclude Include=\"include\\memory.h\" />\n";
    }

    vcxprojFile << R"(  </ItemGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <ClCompile>
      <WarningLevel>Level4</WarningLevel>
      <Optimization>Disabled</Optimization>
      <SDLCheck>true</SDLCheck>
      <ConformanceMode>true</ConformanceMode>
      <AdditionalOptions>/std:c++17 %(AdditionalOptions)</AdditionalOptions>
      <PreprocessorDefinitions>_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <AdditionalDependencies>kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)</AdditionalDependencies>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <ClCompile>
      <WarningLevel>Level4</WarningLevel>
      <Optimization>MaxSpeed</Optimization>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <ConformanceMode>true</ConformanceMode>
      <AdditionalOptions>/std:c++17 %(AdditionalOptions)</AdditionalOptions>
      <PreprocessorDefinitions>NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <AdditionalDependencies>kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)</AdditionalDependencies>
    </Link>
  </ItemDefinitionGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>)";
}

void ProjectGenerator::GenerateFiltersFile() {
    std::ofstream filtersFile(projectPath / (projectName + ".vcxproj.filters"));

    filtersFile << R"(<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <Filter Include="Source Files">
      <UniqueIdentifier>{4FC737F1-C7A5-4376-A066-2A32D752A2FF}</UniqueIdentifier>
      <Extensions>cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx</Extensions>
    </Filter>
    <Filter Include="Header Files">
      <UniqueIdentifier>{93995380-89BD-4b04-88EB-625FBE52EBFB}</UniqueIdentifier>
      <Extensions>h;hh;hpp;hxx;hm;inl;inc;ipp;xsd</Extensions>
    </Filter>
  </ItemGroup>
  <ItemGroup>
    <ClCompile Include="src\main.cpp">
      <Filter>Source Files</Filter>
    </ClCompile>)" << "\n";

    if (projectType == "memory-reader") {
        filtersFile << R"(    <ClCompile Include="src\memory.cpp">
      <Filter>Source Files</Filter>
    </ClCompile>)" << "\n";
    }

    filtersFile << R"(  </ItemGroup>
  <ItemGroup>)" << "\n";

    if (projectType == "memory-reader") {
        filtersFile << R"(    <ClInclude Include="include\memory.h">
      <Filter>Header Files</Filter>
    </ClInclude>)" << "\n";
    }

    filtersFile << R"(  </ItemGroup>
</Project>)";
}

void ProjectGenerator::GenerateReadme() {
    std::ofstream file(projectPath / "README.md");

    file << "# " << projectName << "\n\n";
    file << "## Project Overview\n";
    file << "This project was generated with **NexusCLI**\n\n";

    if (projectType == "memory-reader") {
        file << "## Memory Reader Project\n\n";
        file << "Template for game memory reading applications.\n\n";
        file << "### Features\n";
        file << "- Process memory reading/writing\n";
        file << "- Module base address resolution\n";
        file << "- Template-based memory operations\n\n";
        file << "### Setup\n";
        file << "1. Edit `include/memory.h` to change the target process name\n";
        file << "2. Modify `src/main.cpp` to add your memory reading logic\n";
        file << "3. Update memory addresses for your specific target\n\n";
        file << "### Safety Notice\n";
        file << "- Only use on applications you own or have permission to test\n";
        file << "- Be aware of anti-cheat systems in online games\n";
        file << "- Use responsibly and ethically\n\n";
    }
    else {
        file << "## Console Application\n\n";
        file << "Basic C++ console application template.\n\n";
        file << "### Getting Started\n";
        file << "Edit `src/main.cpp` to add your application logic.\n\n";
    }

    file << "## Building\n";
    file << "1. Open `" << projectName << ".sln` in Visual Studio\n";
    file << "2. Build with **Ctrl+Shift+B**\n";
    file << "3. Run with **F5**\n\n";
    file << "---\n";
    file << "### More Templates & Updates\n";
    file << "**Visit: https://github.com/11Nexuss/NexusCLI**\n";
}