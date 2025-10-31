#include "version.h"
#include <sstream>
#include <vector>
#include <algorithm>

const std::string Version::CURRENT_VERSION = "1.1.0";
const std::string Version::VERSION_URL = "https://raw.githubusercontent.com/11Nexuss/NexusCLI/main/version.txt";
const std::string Version::DOWNLOAD_URL = "https://github.com/11Nexuss/NexusCLI/releases/latest/download/nexus.exe";

bool Version::IsNewerVersion(const std::string& current, const std::string& latest) {
    std::vector<int> currentParts, latestParts;
    std::stringstream currentStream(current);
    std::stringstream latestStream(latest);
    std::string part;

    // Parse current version
    while (std::getline(currentStream, part, '.')) {
        currentParts.push_back(std::stoi(part));
    }

    // Parse latest version
    while (std::getline(latestStream, part, '.')) {
        latestParts.push_back(std::stoi(part));
    }

    // Compare version parts
    for (size_t i = 0; i < std::max(currentParts.size(), latestParts.size()); ++i) {
        int currentPart = (i < currentParts.size()) ? currentParts[i] : 0;
        int latestPart = (i < latestParts.size()) ? latestParts[i] : 0;

        if (latestPart > currentPart) return true;
        if (latestPart < currentPart) return false;
    }

    return false; // Versions are equal
}

std::string Version::GetVersionFromString(const std::string& versionStr) {
    // Extract version number from string (handles "version=1.2.3" or just "1.2.3")
    std::string result = versionStr;

    // Remove "version=" prefix if present
    size_t equalsPos = result.find('=');
    if (equalsPos != std::string::npos) {
        result = result.substr(equalsPos + 1);
    }

    // Trim whitespace
    result.erase(0, result.find_first_not_of(" \t\n\r"));
    result.erase(result.find_last_not_of(" \t\n\r") + 1);

    return result;
}