#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include "Compression.h"      
#include "MemoryManager.h"    
#include "Encryption.h"       
#include "ProcessManager.h"   

namespace fs = std::filesystem;

class FileManager {
public:
    FileManager(); 
    void handleCommand(const std::string& input);

private:
    MemoryManager memoryManager;   
    ProcessManager processManager;  

    void showHelp();
    void listFiles();
    void makeDirectory(const std::string& dirName);
    void remove(const std::string& name);
    void addFile(const std::string& srcPath);
    void createFile(const std::string& filename);
    void readFile(const std::string& filename);
    void writeFile(const std::string& filename);
    void openFile(const std::string& filename);
    bool compress(const std::string& filename);
    bool decompress(const std::string& filename);
    int addProcessTask(const std::string& taskDescription); 
    void clearConsole();
};
