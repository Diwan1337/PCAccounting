#pragma once
#include <vector>
#include <string>

class CryptoService {
public:
    static std::vector<unsigned char> encrypt(
        const std::vector<unsigned char>& data,
        const std::string& password);

    static std::vector<unsigned char> decrypt(
        const std::vector<unsigned char>& data,
        const std::string& password);
};
