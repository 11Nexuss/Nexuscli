#pragma once
#include <string>

class Version {
public:
    static const std::string CURRENT_VERSION;
    static const std::string VERSION_URL;
    static const std::string DOWNLOAD_URL;

    static bool IsNewerVersion(const std::string& current, const std::string& latest);
    static std::string GetVersionFromString(const std::string& versionStr);
};