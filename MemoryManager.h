#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <filesystem> // Required for fs::exists

namespace fs = std::filesystem; // Use the namespace for filesystem

class MemoryManager {
private:
    static const int TOTAL_MEMORY_KB = 1024; // 1 MB (You can adjust this total memory size)
    int usedMemoryKB;
    std::unordered_map<std::string, int> allocations; // filename -> memory used

public:
    MemoryManager();
    void allocate(const std::string& filename, int sizeKB);
    void deallocate(const std::string& filename);
    bool hasAllocation(const std::string& filename) const;
    void displayMemoryUsage() const;
};
