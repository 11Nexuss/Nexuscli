#pragma once
#include <string>

class Updater {
public:
    static bool CheckForUpdates();
    static bool DownloadLatestVersion();
    static std::string GetLatestVersion();
    static bool ReplaceCurrentExecutable(const std::string& newExePath);

private:
    static std::string DownloadString(const std::string& url);
    static bool DownloadFile(const std::string& url, const std::string& outputPath);
};