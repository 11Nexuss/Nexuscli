#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <string_view>
#include <cstdint>

class Memory
{
private:
    std::uintptr_t processId = 0;
    void* processHandle = nullptr;

public:
    Memory(std::wstring_view processName) noexcept;
    ~Memory();

    // Public getters
    std::uintptr_t GetProcessId() const noexcept { return processId; }
    void* GetProcessHandle() const noexcept { return processHandle; }
    bool IsValid() const noexcept { return processHandle != nullptr; }

    std::uintptr_t GetModuleBase(std::wstring_view moduleName) const noexcept;

    template <typename T>
    const T read(const std::uintptr_t address) const noexcept
    {
        T value = { };
        ::ReadProcessMemory(processHandle, reinterpret_cast<const void*>(address), &value, sizeof(T), NULL);
        return value;
    }

    template <typename T>
    void write(const std::uintptr_t address, const T& value) const noexcept
    {
        ::WriteProcessMemory(processHandle, reinterpret_cast<void*>(address), &value, sizeof(T), NULL);
    }
};

// Change the process name to your target
inline Memory* memory = new Memory{ L"cs2.exe" };
inline uintptr_t client = 0;
