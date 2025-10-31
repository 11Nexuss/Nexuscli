#include "updater.h"
#include "version.h"
#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <fstream>
#include <sstream>

#pragma comment(lib, "wininet.lib")

bool Updater::CheckForUpdates() {
    std::cout << "🔍 Checking for updates..." << std::endl;

    try {
        std::string latestVersionStr = GetLatestVersion();
        if (latestVersionStr.empty()) {
            std::cout << "❌ Failed to check for updates" << std::endl;
            return false;
        }

        std::string latestVersion = Version::GetVersionFromString(latestVersionStr);
        std::string currentVersion = Version::CURRENT_VERSION;

        std::cout << "📍 Current version: " << currentVersion << std::endl;
        std::cout << "📦 Latest version: " << latestVersion << std::endl;

        if (Version::IsNewerVersion(currentVersion, latestVersion)) {
            std::cout << std::endl;
            std::cout << "🎉 New version available!" << std::endl;
            std::cout << "Would you like to update? (y/n): ";

            std::string response;
            std::getline(std::cin, response);

            if (response == "y" || response == "Y") {
                return DownloadLatestVersion();
            }
            else {
                std::cout << "Update cancelled." << std::endl;
                return false;
            }
        }
        else {
            std::cout << "✅ You have the latest version!" << std::endl;
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cout << "❌ Error checking for updates: " << e.what() << std::endl;
        return false;
    }
}

std::string Updater::GetLatestVersion() {
    return DownloadString(Version::VERSION_URL);
}

bool Updater::DownloadLatestVersion() {
    std::cout << "📥 Downloading latest version..." << std::endl;

    // Create temp directory
    std::string tempPath = std::string(getenv("TEMP")) + "\\NexusCLI_Update.exe";

    if (DownloadFile(Version::DOWNLOAD_URL, tempPath)) {
        std::cout << "✅ Download successful!" << std::endl;
        std::cout << "🔄 Updating application..." << std::endl;

        return ReplaceCurrentExecutable(tempPath);
    }
    else {
        std::cout << "❌ Download failed!" << std::endl;
        return false;
    }
}

bool Updater::ReplaceCurrentExecutable(const std::string& newExePath) {
    // Get current executable path
    char currentExePath[MAX_PATH];
    GetModuleFileNameA(NULL, currentExePath, MAX_PATH);

    // Create update script
    std::string batScript = std::string(getenv("TEMP")) + "\\nexus_update.bat";
    std::ofstream batFile(batScript);

    if (!batFile) {
        std::cout << "❌ Failed to create update script" << std::endl;
        return false;
    }

    batFile << "@echo off\n";
    batFile << "chcp 65001 >nul\n";
    batFile << "echo Updating NexusCLI...\n";
    batFile << "timeout /t 2 /nobreak >nul\n";
    batFile << "del \"" << currentExePath << "\"\n";
    batFile << "move \"" << newExePath << "\" \"" << currentExePath << "\"\n";
    batFile << "echo Update complete! Starting new version...\n";
    batFile << "timeout /t 1 /nobreak >nul\n";
    batFile << "start \"\" \"" << currentExePath << "\" version\n";
    batFile << "del \"" << batScript << "\"\n";
    batFile << "exit\n";

    batFile.close();

    // Execute the update script
    SHELLEXECUTEINFOA sei = { sizeof(sei) };
    sei.lpVerb = "runas"; // Run as admin if needed
    sei.lpFile = batScript.c_str();
    sei.nShow = SW_HIDE;

    if (ShellExecuteExA(&sei)) {
        std::cout << "🔄 Update started. The application will restart..." << std::endl;
        return true;
    }
    else {
        std::cout << "❌ Failed to start update process" << std::endl;
        return false;
    }
}

std::string Updater::DownloadString(const std::string& url) {
    HINTERNET hInternet = InternetOpenA("NexusCLI Updater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";

    HINTERNET hUrl = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return "";
    }

    std::string result;
    char buffer[1024];
    DWORD bytesRead;

    while (InternetReadFile(hUrl, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }

    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    return result;
}

bool Updater::DownloadFile(const std::string& url, const std::string& outputPath) {
    HINTERNET hInternet = InternetOpenA("NexusCLI Updater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    HINTERNET hUrl = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return false;
    }

    std::ofstream file(outputPath, std::ios::binary);
    if (!file) {
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
        return false;
    }

    char buffer[8192];
    DWORD bytesRead;
    bool success = true;

    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        file.write(buffer, bytesRead);
        if (!file) {
            success = false;
            break;
        }
    }

    file.close();
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    return success;
}