#pragma once
#include <string>
#include <vector>

class Compression {
public:
    static bool compressFile(const std::string& inputFile);
    static bool decompressFile(const std::string& inputFile);

};
