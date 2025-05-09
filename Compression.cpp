#include "Compression.h"
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <cctype>

namespace fs = std::filesystem;

bool Compression::compressFile(const std::string& inputFile) {
    std::ifstream in(inputFile);
    if (!in) {
        std::cerr << "Error opening file for compression: " << inputFile << "\n";
        return false;
    }

    // Prevent double compression
    if (inputFile.length() >= 15 && inputFile.substr(inputFile.length() - 15) == "_compressed.txt") {
        std::cerr << "Error: File '" << inputFile << "' is already compressed.\n";
        in.close();
        return false;
    }

    size_t last_dot_pos = inputFile.find_last_of('.');
    std::string base_name = (last_dot_pos == std::string::npos) ? inputFile : inputFile.substr(0, last_dot_pos);
    std::string outputFile = base_name + "_compressed.txt";

    std::ofstream out(outputFile);
    if (!out) {
        std::cerr << "Error creating compressed file: " << outputFile << "\n";
        in.close();
        return false;
    }

    char currentChar;
    if (!in.get(currentChar)) {
        std::cout << "Input file is empty. Created empty compressed file: " << outputFile << "\n";
        in.close();
        out.close();
        return true;
    }

    int count = 1;
    char nextChar;

    while (in.get(nextChar)) {
        if (nextChar == currentChar) {
            count++;
        }
        else {
            out << currentChar << count;
            currentChar = nextChar;
            count = 1;
        }
    }

    out << currentChar << count;

    in.close();
    out.close();

    std::cout << "File compressed to: " << outputFile << "\n";
    return true;
}

bool Compression::decompressFile(const std::string& inputFile) {
    size_t compressed_suffix_pos = inputFile.find("_compressed.txt");
    if (compressed_suffix_pos == std::string::npos || compressed_suffix_pos != inputFile.length() - 15) {
        std::cerr << "Error: Only files with a '_compressed.txt' suffix can be decompressed.\n";
        return false;
    }

    // Check if file is already decompressed
    if (inputFile.length() >= 15 && inputFile.substr(inputFile.length() - 15) == "_decompressed.txt") {
        std::cerr << "Error: File '" << inputFile << "' is already decompressed.\n";
        return false;
    }

    std::ifstream in(inputFile);
    if (!in) {
        std::cerr << "Error opening file for decompression: " << inputFile << "\n";
        return false;
    }

    std::string baseName = inputFile.substr(0, compressed_suffix_pos);
    std::string outputFile = baseName + "_decompressed.txt";

    std::ofstream out(outputFile);
    if (!out) {
        std::cerr << "Error creating decompressed file: " << outputFile << "\n";
        in.close();
        return false;
    }

    char ch;
    while (in.get(ch)) {
        if (std::isdigit(ch)) {
            std::cerr << "Error: Unexpected digit '" << ch << "' — file may be corrupted.\n";
            out.close();
            in.close();
            return false;
        }

        std::string countStr;
        char digit;
        while (in.get(digit) && std::isdigit(digit)) {
            countStr += digit;
        }

        if (in.good() && !std::isdigit(digit)) {
            in.unget();
        }
        else if (in.fail() && !in.eof()) {
            std::cerr << "Error reading count after '" << ch << "'. File might be corrupted.\n";
            out.close(); in.close(); return false;
        }

        if (countStr.empty()) {
            std::cerr << "Error: Missing count for character '" << ch << "'.\n";
            out.close(); in.close(); return false;
        }

        int count;
        try {
            count = std::stoi(countStr);
        }
        catch (...) {
            std::cerr << "Error: Invalid count '" << countStr << "'.\n";
            out.close(); in.close(); return false;
        }

        for (int i = 0; i < count; ++i) out << ch;
    }

    if (in.fail() && !in.eof()) {
        std::cerr << "Error reading compressed data. File might be corrupted.\n";
        out.close(); in.close(); return false;
    }

    in.close(); out.close();
    std::cout << "File decompressed to: " << outputFile << "\n";
    return true;
}
