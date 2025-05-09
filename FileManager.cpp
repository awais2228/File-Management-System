#include "FileManager.h"
#include "Compression.h"
#include "Encryption.h"
#include "MemoryManager.h"
#include "ProcessManager.h" // Include ProcessManager header
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <limits>
#include <string>
#include <cctype>
#include <vector>

namespace fs = std::filesystem;

// Constructor (if you have any initialization, do it here)
FileManager::FileManager() {
    // Constructor initializes member objects (memoryManager, processManager)
    // Their default constructors are called automatically.
    // You can add any other initialization logic here if needed.
}

// Helper function to add a task to the process manager and return its ID
int FileManager::addProcessTask(const std::string& taskDescription) {
    return processManager.addProcess(taskDescription);
}


void FileManager::handleCommand(const std::string& input) {
    std::istringstream iss(input);
    std::string command;
    iss >> command;

    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }

    // Keep track of process ID for status update
    int processId = -1; // Initialize with an invalid ID

    if (command == "help") {
        showHelp();
        std::cout << "\n"; // Add space after command output
    }
    else if (command == "list") {
        listFiles();
        std::cout << "\n"; // Add space after command output
    }
    else if (command == "mkdir") {
        if (args.empty()) {
            std::cout << "Usage: mkdir <dir>\n\n"; // Add space to usage
        }
        else {
            // Execute command logic first
            makeDirectory(args[0]); // makeDirectory handles existence check and prints error

            // Add process task and update status
            processId = addProcessTask("Create Directory: " + args[0]);
            // Assuming makeDirectory succeeds if it doesn't print an error
            // A more robust approach would be to have makeDirectory return a status
            // For now, we'll assume completion unless an explicit error is caught.
            processManager.updateProcessStatus(processId, ProcessStatus::Completed);
            std::cout << "\n"; // Add space after command output
        }
    }

    else if (command == "touch") {
        if (args.empty()) {
            std::cerr << "Usage: touch <filename>\n\n"; // Add space to usage
            return;
        }
        std::string filename = args[0];
        // Check if file exists before attempting to create
        bool fileExists = fs::exists(filename);

        // Add process task early
        processId = addProcessTask("Create File: " + filename);

        // --- Execute command logic ---
        if (fileExists) {
            std::cerr << "Error: File '" << filename << "' already exists.\n";
            processManager.updateProcessStatus(processId, ProcessStatus::Failed); // Mark as failed
        }
        else {
            createFile(filename); // createFile handles creation and prints messages
            // Assuming createFile succeeds if it doesn't print an error
            processManager.updateProcessStatus(processId, ProcessStatus::Completed);
        }
        // --- End of command logic ---
        std::cout << "\n"; // Add space after command output
    }

    else if (command == "rm") {
        if (args.empty()) {
            std::cout << "Usage: rm <file/dir>\n\n"; // Add space to usage
        }
        else {
            // Check if file exists before attempting removal (and deallocation)
            fs::path target = fs::current_path() / args[0];
            bool fileExists = fs::exists(target);

            // Execute command logic first
            // The remove function handles deallocation attempt and prints messages (including file not found error)
            remove(args[0]);

            // Add process task and update status
            processId = addProcessTask("Remove File/Directory: " + args[0]);

            // Update status based on whether the file existed before attempting removal
            if (fileExists) {
                // Assuming remove succeeds if it doesn't print a filesystem error
                processManager.updateProcessStatus(processId, ProcessStatus::Completed);
            }
            else {
                // If the file didn't exist, the remove operation failed
                processManager.updateProcessStatus(processId, ProcessStatus::Failed);
            }
            std::cout << "\n"; // Add space after command output
        }
    }
    else if (command == "add") {
        if (args.empty()) {
            std::cout << "Usage: add <full_path_to_file>\n\n"; // Add space to usage
        }
        else {
            std::string srcPath = args[0];
            // Check if source file exists before attempting to add
            bool sourceExists = fs::exists(srcPath);

            // Execute command logic
            addFile(srcPath); // addFile handles source file existence check and prints messages

            // Add process task and update status
            processId = addProcessTask("Add File: " + srcPath);

            // Update status based on whether the source file existed
            if (sourceExists) {
                // Assuming addFile succeeds if it doesn't print a filesystem error
                processManager.updateProcessStatus(processId, ProcessStatus::Completed);
            }
            else {
                processManager.updateProcessStatus(processId, ProcessStatus::Failed);
            }
            std::cout << "\n"; // Add space after command output
        }
    }
    else if (command == "read") {
        if (args.empty()) {
            std::cout << "Usage: read <filename>\n\n"; // Add space to usage
        }
        else {
            std::string filename = args[0];
            // Check if file exists before attempting to read
            bool fileExists = fs::exists(filename);

            // Execute command logic
            readFile(filename); // readFile handles file existence check and prints error

            // Reading is usually quick, maybe don't track as a process?
            // If you want to track:
            processId = addProcessTask("Read File: " + filename);
            processManager.updateProcessStatus(processId, fileExists ? ProcessStatus::Completed : ProcessStatus::Failed);

            std::cout << "\n"; // Add space after command output
        }
    }
    else if (command == "write") {
        if (args.empty()) {
            std::cout << "Usage: write <filename>\n\n"; // Add space to usage
        }
        else {
            std::string filename = args[0];
            // Check if file exists before writing
            bool fileExists = fs::exists(filename);

            // Add process task early
            processId = addProcessTask("Write File: " + filename);

            // --- Execute command logic ---
            if (!fileExists) {
                std::cerr << "Error: File '" << filename << "' does not exist. Use 'touch' to create it first.\n";
                processManager.updateProcessStatus(processId, ProcessStatus::Failed); // Mark as failed
            }
            else {
                writeFile(filename); // writeFile handles opening and writing
                // Assuming writeFile succeeds if it doesn't print an error
                processManager.updateProcessStatus(processId, ProcessStatus::Completed);
            }
            // --- End of command logic ---
            std::cout << "\n"; // Add space after command output
        }
    }

    else if (command == "open") {
        if (args.empty()) {
            std::cout << "Usage: open <filename>\n\n"; // Add space to usage
        }
        else {
            std::string filename = args[0];
            // Check if file exists before attempting to open
            bool fileExists = fs::exists(filename);

            // Execute command logic
            openFile(filename); // openFile handles file existence check and prints error

            // Opening is external, maybe don't track as a process?
            // If you want to track:
            processId = addProcessTask("Open File: " + filename);
            processManager.updateProcessStatus(processId, fileExists ? ProcessStatus::Completed : ProcessStatus::Failed);

            std::cout << "\n"; // Add space after command output
        }
    }

    else if (command == "compress") {
        if (args.empty()) {
            std::cout << "Usage: compress <filename>\n\n"; // Add space to usage
        }
        else {
            std::string filename = args[0];
            bool fileExists = fs::exists(filename);
            processId = addProcessTask("Compress File: " + filename);

            if (!fileExists) {
                std::cerr << "Error: File '" << filename << "' does not exist. Cannot compress.\n";
                processManager.updateProcessStatus(processId, ProcessStatus::Failed);
            }
            else {
                bool success = compress(filename);  // new logic
                if (success) {
                    processManager.updateProcessStatus(processId, ProcessStatus::Completed);
                }
                else {
                    processManager.updateProcessStatus(processId, ProcessStatus::Failed);
                }
            }
            std::cout << "\n";
        }
    }
    else if (command == "decompress") {
        if (args.empty()) {
            std::cout << "Usage: decompress <filename>\n\n"; // Add space to usage
        }
        else {
            std::string filename = args[0];
            // Check if file exists before decompressing
            bool fileExists = fs::exists(filename);

            // Add process task early
            processId = addProcessTask("Decompress File: " + filename);

            // --- Execute command logic ---
            if (!fileExists) {
                std::cerr << "Error: File '" << filename << "' does not exist. Cannot decompress.\n";
                processManager.updateProcessStatus(processId, ProcessStatus::Failed); // Mark as failed
            }
            else {
                // decompress calls Compression::decompressFile which returns bool
                bool success = decompress(filename);
                if (success) {
                    processManager.updateProcessStatus(processId, ProcessStatus::Completed);
                }
                else {
                    processManager.updateProcessStatus(processId, ProcessStatus::Failed);
                }
            }
            // --- End of command logic ---
            std::cout << "\n"; // Add space after command output
        }
    }
    else if (command == "encrypt") {
        if (args.size() < 3) {
            std::cout << "Usage: encrypt <algorithm_number> <filename> <key>\n";
            std::cout << "Available algorithms: 1:Caesar, 2:XOR, 3:Vigenere, 4:RailFence\n\n"; // Add space to usage
        }
        else {
            std::string algorithm_number = args[0];
            std::string filename = args[1];
            std::string key = args[2];
            std::string algorithm_name;

            if (algorithm_number == "1") {
                algorithm_name = "caesar";
            }
            else if (algorithm_number == "2") {
                algorithm_name = "xor";
            }
            else if (algorithm_number == "3") {
                algorithm_name = "vigenere";
            }
            else if (algorithm_number == "4") {
                algorithm_name = "railfence";
            }
            else {
                std::cerr << "Invalid algorithm number. Please use 1, 2, 3, or 4.\n\n"; // Add space to error
                // No process added for invalid algorithm number, so no status update needed
                return;
            }
            // Add process task early
            processId = addProcessTask("Encrypt File: " + filename + " (" + algorithm_name + ")");

            // --- Check if file exists before encrypting ---
            if (!fs::exists(filename)) {
                std::cerr << "Error: File '" << filename << "' does not exist. Cannot encrypt.\n";
                processManager.updateProcessStatus(processId, ProcessStatus::Failed); // Mark as failed
                std::cout << "\n"; // Add space after command output
                return;
            }
            // --- End of check ---

            // Execute command logic
            bool success = Encryption::encryptFile(algorithm_name, filename, key);
            if (success) {
                processManager.updateProcessStatus(processId, ProcessStatus::Completed);
            }
            else {
                processManager.updateProcessStatus(processId, ProcessStatus::Failed);
            }

            std::cout << "\n"; // Add space after command output
        }
    }
    else if (command == "decrypt") {
        if (args.size() < 3) {
            std::cout << "Usage: decrypt <algorithm_number> <filename> <key>\n";
            std::cout << "Available algorithms: 1:Caesar, 2:XOR, 3:Vigenere, 4:RailFence\n\n"; // Add space to usage
        }
        else {
            std::string algorithm_number = args[0];
            std::string filename = args[1];
            std::string key = args[2];
            std::string algorithm_name;

            if (algorithm_number == "1") {
                algorithm_name = "caesar";
            }
            else if (algorithm_number == "2") {
                algorithm_name = "xor";
            }
            else if (algorithm_number == "3") {
                algorithm_name = "vigenere";
            }
            else if (algorithm_number == "4") {
                algorithm_name = "railfence";
            }
            else {
                std::cerr << "Invalid algorithm number. Please use 1, 2, 3, or 4.\n\n"; // Add space to error
                // No process added for invalid algorithm number, so no status update needed
                return;
            }
            // Add process task early
            processId = addProcessTask("Decrypt File: " + filename + " (" + algorithm_name + ")");

            // --- Check if file exists before decrypting ---
            if (!fs::exists(filename)) {
                std::cerr << "Error: File '" << filename << "' does not exist. Cannot decrypt.\n";
                processManager.updateProcessStatus(processId, ProcessStatus::Failed); // Mark as failed
                std::cout << "\n"; // Add space after command output
                return;
            }
            // --- End of check ---
            // Execute command logic
            // Encryption::decryptFile also has an internal check for the .enc suffix
            bool success = Encryption::decryptFile(algorithm_name, filename, key);
            if (success) {
                processManager.updateProcessStatus(processId, ProcessStatus::Completed);
            }
            else {
                processManager.updateProcessStatus(processId, ProcessStatus::Failed);
            }
            std::cout << "\n"; // Add space after command output
        }
    }
    else if (command == "alloc") {
        if (args.size() < 2) {
            std::cout << "Usage: alloc <filename> <sizeKB>\n\n"; // Add space to usage
        }
        else {
            std::string filename = args[0];
            // Check if file exists before allocating
            bool fileExists = fs::exists(filename);

            // Add process task early
            processId = addProcessTask("Allocate Memory: " + filename + " (" + args[1] + "KB)");

            // --- Execute command logic ---
            if (!fileExists) {
                std::cerr << "Error: File '" << filename << "' does not exist. Cannot allocate memory.\n";
                processManager.updateProcessStatus(processId, ProcessStatus::Failed); // Mark as failed
            }
            else {
                try {
                    int sizeKB = std::stoi(args[1]);
                    memoryManager.allocate(filename, sizeKB); // Call allocate on the member variable
                    // memoryManager.allocate prints success/failure messages related to allocation logic
                    // We need to check the result of allocate to determine process status more accurately
                    // For now, we'll assume if file exists and size is valid, it attempts allocation.
                    // A more robust MM::allocate would return bool.
                    processManager.updateProcessStatus(processId, ProcessStatus::Completed); // Assuming success if file exists and size is valid
                }
                catch (const std::invalid_argument) {
                    std::cerr << "Error: Invalid size. Please provide an integer value for sizeKB.\n";
                    processManager.updateProcessStatus(processId, ProcessStatus::Failed); // Mark as failed
                }
                catch (const std::out_of_range) {
                    std::cerr << "Error: Size value out of range for integer conversion.\n";
                    processManager.updateProcessStatus(processId, ProcessStatus::Failed); // Mark as failed
                }
            }
            // --- End of command logic ---
            std::cout << "\n"; // Add space after command output
        }
    }
    else if (command == "dealloc") {
        if (args.empty()) {
            std::cout << "Usage: dealloc <filename>\n\n"; // Add space to usage
        }
        else {
            std::string filename = args[0];
            // Check if file exists before deallocating
            bool fileExists = fs::exists(filename);

            // Add process task early
            processId = addProcessTask("Deallocate Memory: " + filename);

            // --- Execute command logic ---
            if (!fileExists) {
                std::cerr << "Error: File '" << filename << "' does not exist. Cannot deallocate memory.\n";
                processManager.updateProcessStatus(processId, ProcessStatus::Failed); // Mark as failed
            }
            else {
                // memoryManager.deallocate is silent if no allocation exists.
                // It doesn't return a status. We'll assume success if the file exists.
                memoryManager.deallocate(filename);
                processManager.updateProcessStatus(processId, ProcessStatus::Completed); // Assuming success if file exists
            }
            // --- End of command logic ---
            std::cout << "\n"; // Add space after command output
        }
    }
    else if (command == "meminfo") {
        memoryManager.displayMemoryUsage();
        std::cout << "\n"; // Add space after command output
    }
    else if (command == "procstatus") { // New command to show process status
        processManager.showStatus();
        std::cout << "\n"; // Add space after command output
    }
    else if (command == "clearcp") { // New command to clear completed processes
        processManager.clearCompleted();
        std::cout << "\n"; // Add space after command output
    }
    else if (command == "clearallp") { // New command to clear all processes
        processManager.clearAll();
        std::cout << "\n"; // Add space after command output
    }

    else if (command == "clear") {
        clearConsole();
    }
    else if (command == "exit") {
        std::exit(0);
    }
    else {
        std::cout << "Unknown command. Type 'help' for available commands.\n\n"; // Add space to unknown command
    }
}

void FileManager::showHelp() {
    std::cout << "Available Commands:\n";
    std::cout << "  help                           - Show this help menu\n";
    std::cout << "  list                           - List files and directories\n";
    std::cout << "  mkdir <dir>                    - Create a new directory\n";
    std::cout << "  rm <file/dir>                  - Delete a file or directory\n";
    std::cout << "  add <filepath>                 - Add file to working directory\n";
    std::cout << "  touch <file>                   - Create a new empty file\n";
    std::cout << "  read <filename>                - Read and display file content\n";
    std::cout << "  write <filename>               - Write content to an existing file\n"; // Updated help for write
    std::cout << "  open <filename>                - Open a file using the default application\n";
    std::cout << "  compress <file>                - Compress a text file using RLE\n";
    std::cout << "  decompress <file>              - Decompress an RLE-compressed file\n";
    std::cout << "  encrypt <algo> <file> <key>    - Encrypt a file using algorithm\n";
    std::cout << "  decrypt <algo> <file> <key>    - Decrypt a file using algorithm\n";
    std::cout << "  alloc <file> <sizeKB>          - Allocate memory to an existing file\n"; // Updated help for alloc
    std::cout << "  dealloc <file>                 - Deallocate memory from an existing file\n"; // Updated help for dealloc
    std::cout << "  meminfo                        - Show memory usage and allocations\n";
    std::cout << "  procstatus                     - Show the status of background processes\n"; // Added help for process status
    std::cout << "  clearcp                        - Clear completed processes from the queue\n"; // Added help for clear completed processes
    std::cout << "  clearallp                      - Clear all processes from the queue\n"; // Added help for clear all processes
    std::cout << "  clear                          - Clears the Console\n";
    std::cout << "  exit                           - Exit the CLI\n";
    std::cout << "    Available algorithms:\n";
    std::cout << "      1: Caesar\n";
    std::cout << "      2: XOR\n";
    std::cout << "      3: Vigenere\n";
    std::cout << "      4: Rail Fence\n";
    std::cout << "\n"; // Add space after help
}

void FileManager::listFiles() {
    // Error handling for directory_iterator in case current_path() is not accessible
    try {
        for (const auto& entry : fs::directory_iterator(fs::current_path())) {
            if (entry.is_directory()) {
                std::cout << "[DIR]  " << entry.path().filename().string() << '\n';
            }
            else {
                std::cout << "       " << entry.path().filename().string() << '\n';
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error listing directory: " << e.what() << std::endl;
    }
}

void FileManager::makeDirectory(const std::string& dirName) {
    if (fs::create_directory(dirName)) {
        std::cout << "Directory '" << dirName << "' created.\n";
    }
    else {
        std::cerr << "Failed to create directory '" << dirName << "' or it already exists.\n"; // Use cerr for errors
    }
}

void FileManager::remove(const std::string& name) {
    try {
        fs::path target = fs::current_path() / name;

        // Attempt to deallocate memory before removing the file from the filesystem
        // MM::deallocate handles if no memory was allocated, so it's safe to call.
        bool wasAllocated = memoryManager.hasAllocation(name);
        memoryManager.deallocate(name);

        // If memory was allocated, print a confirmation message
        if (wasAllocated) {
            std::cout << "Memory allocated for '" << name << "' has been deallocated.\n";
        }

        if (!fs::exists(target)) {
            std::cerr << "Error: File or directory '" << name << "' does not exist.\n"; // Use cerr for errors
            return;
        }

        if (fs::is_directory(target)) {
            // Note: fs::remove_all will remove the directory and its contents.
            // If you allocated memory to files *within* the directory,
            // those allocations in MemoryManager will NOT be automatically deallocated
            // unless you iterate through the directory contents here and call deallocate for each file.
            // For this current implementation, only the allocation for the directory name itself (if any) is handled.
            fs::remove_all(target);
            std::cout << "Directory '" << name << "' removed.\n";
        }
        else {
            fs::remove(target);
            std::cout << "File '" << name << "' removed.\n";
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error removing '" << name << "': " << e.what() << '\n';
    }
}

void FileManager::addFile(const std::string& srcPath) {
    try {
        fs::path source(srcPath);
        fs::path destination = fs::current_path() / source.filename();

        if (!fs::exists(source)) {
            std::cerr << "Error: Source file '" << srcPath << "' does not exist.\n"; // Use cerr for errors
            return;
        }

        fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
        std::cout << "File '" << source.filename().string() << "' added successfully to working directory.\n";
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error adding file '" << srcPath << "': " << e.what() << '\n';
    }
}

void FileManager::createFile(const std::string& filename) {
    // This function is now called ONLY if the file doesn't exist (checked in handleCommand)
    std::ofstream file(filename);
    if (file) {
        std::cout << "File created: '" << filename << "'\n";
        file.close();
    }
    else {
        // This error indicates a problem creating a *new* file
        std::cerr << "Failed to create file: '" << filename << "'\n";
    }
}

void FileManager::readFile(const std::string& filename) {
    // Check if file exists before attempting to open
    if (!fs::exists(filename)) {
        std::cerr << "Error: File '" << filename << "' not found.\n";
        return;
    }

    std::ifstream file(filename);

    if (file.is_open()) {
        std::cout << "--- Content of '" << filename << "' ---\n";
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << '\n';
        }
        std::cout << "-----------------------------\n";
        file.close();
    }
    else {
        // This case might be less likely after fs::exists check, but good to keep
        std::cerr << "Error: Could not open file '" << filename << "' for reading.\n";
    }
}

void FileManager::writeFile(const std::string& filename) {
    // This function is now called ONLY if the file exists (checked in handleCommand)
    std::ofstream file(filename, std::ios::out); // std::ios::out truncates the file

    if (file.is_open()) {
        std::cout << "Enter content to write to '" << filename << "'. Press Enter on a new line followed by 'EOF' to save:\n";
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "EOF") {
                break;
            }
            file << line << '\n';
        }
        file.close();
        std::cout << "Content written to '" << filename << "'.\n";
    }
    else {
        // This error indicates a problem opening an *existing* file for writing
        std::cerr << "Error: Could not open existing file '" << filename << "' for writing.\n";
    }
}

void FileManager::openFile(const std::string& filename) {
    fs::path filePath = fs::current_path() / filename;

    if (!fs::exists(filePath)) {
        std::cerr << "Error: File '" << filename << "' not found.\n";
        return;
    }

    std::string command;

#ifdef _WIN32
    command = "start \"\" \"" + filePath.string() + "\"";
#elif __APPLE__
    command = "open \"" + filePath.string() + "\"";
#else
    command = "xdg-open \"" + filePath.string() + "\"";
#endif

    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Warning: System command failed (" << result << "). Could not open file '" << filename << "' or the command encountered an issue.\n";
    }
    else {
        std::cout << "Attempting to open file '" << filename << "' using default application...\n";
    }
}

// Assuming compress and decompress are in Compression class as per your file structure
bool FileManager::compress(const std::string& filename) {
    // File existence checked in handleCommand
     return Compression::compressFile(filename);
}

// Calls Compression::decompressFile which now returns bool
bool FileManager::decompress(const std::string& filename) {
    // File existence checked in handleCommand
    return Compression::decompressFile(filename); // Return the bool result
}


void FileManager::clearConsole() {
    std::system("cls");
}
