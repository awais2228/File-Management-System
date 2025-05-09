#include "FileManager.h"
#include <iostream>
#include <string>

int main() {
    FileManager manager;
    std::string input;

    std::cout << "Simple File Manager CLI\n";
    std::cout << "Type 'help' to see available commands. Type 'exit' to quit.\n\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        if (!input.empty()) {
            manager.handleCommand(input);
        }
    }
    return 0;
}
