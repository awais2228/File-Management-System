#include "Encryption.h"
#include <fstream>
#include <iostream>
#include <cctype>
#include <vector>
#include <string>
#include <limits>
#include <stdexcept> 

std::string Encryption::readFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return "";

    return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

void Encryption::writeFile(const std::string& filename, const std::string& content) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Error writing to file: " << filename << "\n";
        return; // Return void on failure
    }
    out << content;
}

// --- Caesar Cipher ---
std::string Encryption::caesarEncrypt(const std::string& text, int shift) {
    std::string result = "";
    shift = shift % 26;
    if (shift < 0) shift += 26;
    for (char ch : text) {
        if (std::isalpha(ch)) {
            if (std::islower(ch)) {
                result += (char)(((ch - 'a' + shift) % 26) + 'a');
            }
            else {
                result += (char)(((ch - 'A' + shift) % 26) + 'A');
            }
        }
        else {
            result += ch;
        }
    }
    return result;
}

std::string Encryption::caesarDecrypt(const std::string& text, int shift) {
    shift = shift % 26;
    if (shift < 0) shift += 26;
    return caesarEncrypt(text, 26 - shift); // Decrypting is encrypting with inverse shift
}

// --- XOR Cipher ---
std::string Encryption::xorCipher(const std::string& text, const std::string& key) {
    std::string result = text;
    // Basic XOR, key can be any characters
    if (key.empty()) return text; // Return original text if key is empty

    for (size_t i = 0; i < text.size(); ++i) {
        result[i] ^= key[i % key.size()];
    }
    return result;
}

// --- Vigenere Cipher ---
std::string Encryption::vigenereEncrypt(const std::string& text, const std::string& key) {
    std::string result = "";
    std::string clean_key = "";
    // Clean key: keep only alphabetic characters and convert to lowercase
    for (char k : key) {
        if (std::isalpha(k)) {
            clean_key += std::tolower(k);
        }
    }

    if (clean_key.empty()) {
        // If key is empty or contains no alphabetic chars, return original text
        return text;
    }

    size_t key_index = 0;
    for (char ch : text) {
        if (std::isalpha(ch)) {
            char key_char = clean_key[key_index % clean_key.length()];
            int shift = key_char - 'a';

            if (std::islower(ch)) {
                result += (char)(((ch - 'a' + shift) % 26) + 'a');
            }
            else {
                result += (char)(((ch - 'A' + shift) % 26) + 'A');
            }
            key_index++; // Move to the next key character only for alphabetic text characters
        }
        else {
            result += ch; // Keep non-alphabetic characters as they are
        }
    }
    return result;
}

std::string Encryption::vigenereDecrypt(const std::string& text, const std::string& key) {
    std::string result = "";
    std::string clean_key = "";
    // Clean key: keep only alphabetic characters and convert to lowercase
    for (char k : key) {
        if (std::isalpha(k)) {
            clean_key += std::tolower(k);
        }
    }
    if (clean_key.empty()) {
        // If key is empty or contains no alphabetic chars, return original text
        return text;
    }
    size_t key_index = 0;
    for (char ch : text) {
        if (std::isalpha(ch)) {
            char key_char = clean_key[key_index % clean_key.length()];
            int shift = key_char - 'a';

            if (std::islower(ch)) {
                result += (char)(((ch - 'a' - shift + 26) % 26) + 'a');
            }
            else {
                result += (char)(((ch - 'A' - shift + 26) % 26) + 'A');
            }
            key_index++; // Move to the next key character only for alphabetic text characters
        }
        else {
            result += ch; // Keep non-alphabetic characters as they are
        }
    }
    return result;
}

// --- Rail Fence Cipher ---
std::string Encryption::railFenceEncrypt(const std::string& text, int rails) {
    if (rails <= 1 || text.empty()) return text; // Need at least 2 rails

    std::vector<std::string> fence(rails);
    int rail = 0;
    int direction = 1; // 1 for down, -1 for up

    for (char ch : text) {
        fence[rail] += ch;
        if (rail == 0) direction = 1;
        else if (rail == rails - 1) direction = -1;
        rail += direction;
    }

    std::string result = "";
    for (const auto& row : fence) {
        result += row;
    }
    return result;
}

std::string Encryption::railFenceDecrypt(const std::string& text, int rails) {
    if (rails <= 1 || text.empty()) return text; // Need at least 2 rails

    std::vector<std::string> fence(rails);
    std::vector<int> rail_lengths(rails, 0);
    int rail = 0;
    int direction = 1; // 1 for down, -1 for up

    // Calculate the length of each rail
    for (char ch : text) {
        rail_lengths[rail]++;
        if (rail == 0) direction = 1;
        else if (rail == rails - 1) direction = -1;
        rail += direction;
    }

    // Fill the fence with parts of the text based on calculated lengths
    size_t current_pos = 0;
    for (int i = 0; i < rails; ++i) {
        fence[i] = text.substr(current_pos, rail_lengths[i]);
        current_pos += rail_lengths[i];
    }

    // Read the fence zig-zag to get the original text
    std::string result = "";
    rail = 0;
    direction = 1;
    std::vector<int> rail_indices(rails, 0); // To keep track of current position in each rail's string
    for (size_t i = 0; i < text.length(); ++i) {
        result += fence[rail][rail_indices[rail]++];
        if (rail == 0) direction = 1;
        else if (rail == rails - 1) direction = -1;
        rail += direction;
    }
    return result;
}


// --- Encryption ---
bool Encryption::encryptFile(const std::string& algorithm, const std::string& filename, const std::string& key) {
    std::cout << "Encrypting '" << filename << "' using " << algorithm << " with key '" << key << "'...\n";

    std::string content = readFile(filename);
    if (content.empty()) {
        std::cerr << "Error: Could not read content from " << filename << ". Encryption failed.\n";
        return false;
    }

    std::string result;
    std::string algo = algorithm;
    for (auto& c : algo) c = std::tolower(c); // Convert algorithm name to lowercase

    try {
        if (algo == "caesar") {
            // Validate key for Caesar: must be an integer
            try {
                size_t pos;
                int shift = std::stoi(key, &pos);
                if (pos != key.size()) { // Check if the entire string was consumed by stoi
                    std::cerr << "Error: Invalid key format for Caesar cipher. Key must be an integer.\n";
                    return false;
                }
                result = caesarEncrypt(content, shift);
            }
            catch (const std::invalid_argument) {
                std::cerr << "Error: Invalid key format for Caesar cipher. Key must be an integer.\n";
                return false;
            }
            catch (const std::out_of_range) {
                std::cerr << "Error: Key value out of range for Caesar cipher.\n";
                return false;
            }
        }
        else if (algo == "xor") {
            // XOR key can be any string, but empty key is handled
            if (key.empty()) {
                std::cerr << "Error: Key cannot be empty for XOR cipher.\n";
                return false;
            }
            result = xorCipher(content, key);
        }
        else if (algo == "vigenere") {
            // Validate key for Vigenere: must contain at least one alphabetic character
            bool has_alpha = false;
            for (char c : key) {
                if (std::isalpha(c)) {
                    has_alpha = true;
                    break;
                }
            }
            if (key.empty() || !has_alpha) {
                std::cerr << "Error: Key for Vigenere cipher must contain at least one alphabetic character.\n";
                return false;
            }
            result = vigenereEncrypt(content, key);
        }
        else if (algo == "railfence") {
            // Validate key for Rail Fence: must be an integer > 1
            try {
                size_t pos;
                int rails = std::stoi(key, &pos);
                if (pos != key.size()) { // Check if the entire string was consumed by stoi
                    std::cerr << "Error: Invalid key format for Rail Fence cipher. Key must be an integer.\n";
                    return false;
                }
                if (rails <= 1) {
                    std::cerr << "Error: Invalid key for Rail Fence. Number of rails must be greater than 1.\n";
                    return false;
                }
                result = railFenceEncrypt(content, rails);
            }
            catch (const std::invalid_argument) {
                std::cerr << "Error: Invalid key format for Rail Fence cipher. Key must be an integer.\n";
                return false;
            }
            catch (const std::out_of_range) {
                std::cerr << "Error: Key value out of range for Rail Fence cipher.\n";
                return false;
            }
        }
        else {
            std::cerr << "Error: Unknown encryption algorithm.\n";
            return false;
        }
    }
    catch (const std::exception& e) {
        // Catch any other potential exceptions during encryption logic
        std::cerr << "An unexpected error occurred during encryption: " << e.what() << "\n";
        return false;
    }
    catch (...) {
        // Catch any other unknown exceptions
        std::cerr << "An unknown error occurred during encryption.\n";
        return false;
    }


    // Construct output filename: base_name_algorithm.enc
    size_t last_dot_pos = filename.find_last_of('.');
    std::string base_name = (last_dot_pos == std::string::npos) ? filename : filename.substr(0, last_dot_pos);
    std::string outFile = base_name + "_" + algo + ".enc";

    writeFile(outFile, result);
    std::cout << "File encrypted to: " << outFile << "\n";
    return true; // Return true on success
}

// --- Decryption ---
bool Encryption::decryptFile(const std::string& algorithm, const std::string& filename, const std::string& key) {
    std::cout << "Decrypting '" << filename << "' using " << algorithm << " with key '" << key << "'...\n";

    std::string content = readFile(filename);
    if (content.empty()) {
        std::cerr << "Decryption failed: File is empty or unreadable.\n";
        return false;
    }

    std::string algo = algorithm;
    for (auto& c : algo) c = std::tolower(c); // Convert algorithm name to lowercase

    // Expected filename suffix based on the algorithm
    std::string expected_suffix = "_" + algo + ".enc";

    // Enforce filename-algorithm match and .enc extension
    if (filename.length() < expected_suffix.length() ||
        filename.substr(filename.length() - expected_suffix.length()) != expected_suffix) {
        std::cerr << "Decryption failed: File '" << filename << "' does not match expected encrypted file format for '" << algorithm << "' (expected suffix '" << expected_suffix << "').\n";
        return false;
    }


    std::string result;
    try {
        if (algo == "caesar") {
            // Validate key for Caesar: must be an integer
            try {
                size_t pos;
                int shift = std::stoi(key, &pos);
                if (pos != key.size()) { // Check if the entire string was consumed by stoi
                    std::cerr << "Error: Invalid key format for Caesar cipher. Key must be an integer.\n";
                    return false;
                }
                result = caesarDecrypt(content, shift);
            }
            catch (const std::invalid_argument) {
                std::cerr << "Error: Invalid key format for Caesar cipher. Key must be an integer.\n";
                return false;
            }
            catch (const std::out_of_range) {
                std::cerr << "Error: Key value out of range for Caesar cipher.\n";
                return false;
            }
        }
        else if (algo == "xor") {
            // XOR key can be any string, but empty key is handled
            if (key.empty()) {
                std::cerr << "Error: Key cannot be empty for XOR cipher.\n";
                return false;
            }
            result = xorCipher(content, key);
        }
        else if (algo == "vigenere") {
            // Validate key for Vigenere: must contain at least one alphabetic character
            bool has_alpha = false;
            for (char c : key) {
                if (std::isalpha(c)) {
                    has_alpha = true;
                    break;
                }
            }
            if (key.empty() || !has_alpha) {
                std::cerr << "Error: Key for Vigenere cipher must contain at least one alphabetic character.\n";
                return false;
            }
            result = vigenereDecrypt(content, key);
        }
        else if (algo == "railfence") {
            // Validate key for Rail Fence: must be an integer > 1
            try {
                size_t pos;
                int rails = std::stoi(key, &pos);
                if (pos != key.size()) { // Check if the entire string was consumed by stoi
                    std::cerr << "Error: Invalid key format for Rail Fence cipher. Key must be an integer.\n";
                    return false;
                }
                if (rails <= 1) {
                    std::cerr << "Error: Invalid key for Rail Fence. Number of rails must be greater than 1.\n";
                    return false;
                }
                result = railFenceDecrypt(content, rails);
            }
            catch (const std::invalid_argument) {
                std::cerr << "Error: Invalid key format for Rail Fence cipher. Key must be an integer.\n";
                return false;
            }
            catch (const std::out_of_range) {
                std::cerr << "Error: Key value out of range for Rail Fence cipher.\n";
                return false;
            }
        }
        else {
            std::cerr << "Internal Error: Unknown decryption algorithm.\n"; // Should not happen if encrypt works
            return false;
        }
    }
    catch (const std::exception& e) {
        // Catch any other potential exceptions during decryption logic
        std::cerr << "An unexpected error occurred during decryption: " << e.what() << "\n";
        return false;
    }
    catch (...) {
        // Catch any other unknown exceptions
        std::cerr << "An unknown error occurred during decryption.\n";
        return false;
    }


    // Construct output filename: remove the algorithm suffix and .enc, then add .dec.txt
    // Example: file_caesar.enc -> file.dec.txt
    size_t suffix_pos = filename.rfind("_" + algo + ".enc");
    std::string base_name = filename.substr(0, suffix_pos);
    std::string outFile = base_name + ".dec.txt";

    writeFile(outFile, result);
    std::cout << "File decrypted successfully to: " << outFile << "\n";
    return true; // Return true on success
}

