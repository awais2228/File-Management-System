#include "MemoryManager.h"
#include <iostream> // For std::cout, std::cerr
#include <fstream>  // For file operations (though fs::exists is used for checking)
#include <unordered_map> // For std::unordered_map
#include <string>   // For std::string
#include <filesystem> // For fs::exists
MemoryManager::MemoryManager() : usedMemoryKB(0) {
}

// Allocate memory for a file
void MemoryManager::allocate(const std::string& fileName, int sizeKB) {
    // Check if the file exists using filesystem
    if (!fs::exists(fileName)) {
        std::cout << "Error: File '" << fileName << "' does not exist. Cannot allocate memory.\n";
        return;
    }

    // Check if memory is already allocated for this file
    if (allocations.find(fileName) != allocations.end()) {
        std::cout << "Memory already allocated for '" << fileName << "'.\n";
        return;
    }

    // Check if the requested size is valid (positive)
    if (sizeKB <= 0) {
        std::cout << "Error: Allocation size must be positive.\n";
        return;
    }

    // Check if there is enough free memory
    if (usedMemoryKB + sizeKB > TOTAL_MEMORY_KB) {
        std::cout << "Error: Not enough free memory to allocate " << sizeKB << "KB to '" << fileName << "'.\n";
        std::cout << "Available free memory: " << (TOTAL_MEMORY_KB - usedMemoryKB) << " KB\n";
        return;
    }
    allocations[fileName] = sizeKB;
    usedMemoryKB += sizeKB;

    std::cout << "Allocated " << sizeKB << "KB to '" << fileName << "'.\n";
}

void MemoryManager::deallocate(const std::string& fileName) {
    // Find the file in the allocations map
    auto it = allocations.find(fileName);
    if (it == allocations.end()) {
        return;
    }
    int allocatedSize = it->second;
    allocations.erase(it);
    usedMemoryKB -= allocatedSize;
}

// Check if a file has allocated memory
bool MemoryManager::hasAllocation(const std::string& filename) const {
    return allocations.count(filename) > 0;
}


// Display current memory usage and allocations
void MemoryManager::displayMemoryUsage() const {
    std::cout << "\n--- Memory Information ---\n";
    std::cout << "Total Memory: " << TOTAL_MEMORY_KB << " KB\n";
    std::cout << "Used Memory : " << usedMemoryKB << " KB\n";
    std::cout << "Free Memory : " << (TOTAL_MEMORY_KB - usedMemoryKB) << " KB\n";

    std::cout << "\nCurrent Allocations:\n";
    if (allocations.empty()) {
        std::cout << "  No memory allocations.\n";
    }
    else {
        for (const auto& pair : allocations) {
            std::cout << "  File: '" << pair.first << "' -> " << pair.second << " KB\n";
        }
    }
    std::cout << "--------------------------\n";
}
