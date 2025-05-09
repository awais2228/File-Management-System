#pragma once
#include <string>
#include <vector>
class Encryption {
public:
    static bool encryptFile(const std::string& algorithm, const std::string& filename, const std::string& key);
    static bool decryptFile(const std::string& algorithm, const std::string& filename, const std::string& key);

private:
    static std::string readFile(const std::string& filename);
    static void writeFile(const std::string& filename, const std::string& content);

    // Caesar Cipher
    static std::string caesarEncrypt(const std::string& text, int shift);
    static std::string caesarDecrypt(const std::string& text, int shift);

    // XOR Cipher
    static std::string xorCipher(const std::string& text, const std::string& key);

    // Vigenère Cipher
    static std::string vigenereEncrypt(const std::string& text, const std::string& key);
    static std::string vigenereDecrypt(const std::string& text, const std::string& key);

    // Rail Fence Cipher
    static std::string railFenceEncrypt(const std::string& text, int rails);
    static std::string railFenceDecrypt(const std::string& text, int rails);
};
