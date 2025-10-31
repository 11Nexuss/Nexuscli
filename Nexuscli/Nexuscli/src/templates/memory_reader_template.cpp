#include "../template_manager.h"

void TemplateManager::LoadMemoryReaderTemplate() {
    templates["memory-reader"] = {
        {"include/memory.h", R"(#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <string_view>
#include <cstdint>

class Memory {
private:
    std::uintptr_t processId = 0;
    void* processHandle = nullptr;
public:
    Memory(std::wstring_view processName) noexcept;
    ~Memory();
    std::uintptr_t GetProcessId() const noexcept { return processId; }
    void* GetProcessHandle() const noexcept { return processHandle; }
    bool IsValid() const noexcept { return processHandle != nullptr; }
    std::uintptr_t GetModuleBase(std::wstring_view moduleName) const noexcept;
    
    template <typename T>
    const T read(const std::uintptr_t address) const noexcept {
        T value = { };
        ::ReadProcessMemory(processHandle, reinterpret_cast<const void*>(address), &value, sizeof(T), NULL);
        return value;
    }
    
    template <typename T>
    void write(const std::uintptr_t address, const T& value) const noexcept {
        ::WriteProcessMemory(processHandle, reinterpret_cast<void*>(address), &value, sizeof(T), NULL);
    }
};

inline Memory* memory = new Memory{ L"cs2.exe" };
inline uintptr_t client = 0;
)"},
        {"src/memory.cpp", R"(#include "memory.h"

Memory::Memory(std::wstring_view processName) noexcept {
    ::PROCESSENTRY32W entry = { };
    entry.dwSize = sizeof(::PROCESSENTRY32W);
    const HANDLE snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    while (::Process32NextW(snapShot, &entry)) {
        if (!processName.compare(entry.szExeFile)) {
            processId = entry.th32ProcessID;
            processHandle = ::OpenProcess(PROCESS_VM_READ, FALSE, static_cast<DWORD>(processId));
            break;
        }
    }
    if (snapShot) ::CloseHandle(snapShot);
}

Memory::~Memory() {
    if (processHandle) ::CloseHandle(processHandle);
}

std::uintptr_t Memory::GetModuleBase(std::wstring_view moduleName) const noexcept {
    ::MODULEENTRY32W entry = { };
    entry.dwSize = sizeof(::MODULEENTRY32W);
    const HANDLE snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, static_cast<DWORD>(processId));
    std::uintptr_t result = 0;
    while (::Module32NextW(snapShot, &entry)) {
        if (!moduleName.compare(entry.szModule)) {
            result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
            break;
        }
    }
    if (snapShot) ::CloseHandle(snapShot);
    return result;
}
)"},
        {"src/main.cpp", R"(#include "memory.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "{{PROJECT_NAME}} - Memory Reader" << std::endl;
    
    // Wait for process initialization
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    if (!memory->IsValid()) {
        std::cout << "Failed to find target process!" << std::endl;
        std::cout << "Make sure the process is running and check memory.h" << std::endl;
        return -1;
    }
    
    std::cout << "Process found! Process ID: " << memory->GetProcessId() << std::endl;
    
    // Get module base address
    client = memory->GetModuleBase(L"client.dll");
    
    if (client) {
        std::cout << "client.dll base: 0x" << std::hex << client << std::dec << std::endl;
    } else {
        std::cout << "Failed to find client.dll" << std::endl;
    }
    
    // Example memory reading loop
    while (true) {
        // Add your memory reading logic here
        // Example: Read health from game memory
        // uint32_t health = memory->read<uint32_t>(client + 0x123456);
        // std::cout << "Health: " << health << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Break after 10 iterations for demo
        static int counter = 0;
        if (++counter > 10) break;
    }
    
    std::cout << "Memory Reader Stopped." << std::endl;
    return 0;
})"}
    };
}