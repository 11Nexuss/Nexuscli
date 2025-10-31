#include "memory.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "MyGameHack - Memory Reader Started..." << std::endl;
    
    // Wait for process initialization
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    if (!memory->IsValid()) {
        std::cout << "Failed to find target process!" << std::endl;
        std::cout << "Make sure the process is running and check memory.h" << std::endl;
        return -1;
    }
    
    std::cout << "Process found! Process ID: " << memory->GetProcessId() << std::endl;
    
    // Get module base
    client = memory->GetModuleBase(L"client.dll");
    
    if (client) {
        std::cout << "client.dll base: 0x" << std::hex << client << std::dec << std::endl;
    } else {
        std::cout << "Failed to find client.dll" << std::endl;
    }
    
    // Your memory reading logic here
    while (true) {
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
}
