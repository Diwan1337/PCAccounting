#include "CryptoService.h"
#include <windows.h>
#include <bcrypt.h>
#include <stdexcept>
#include <vector>

#pragma comment(lib, "bcrypt.lib")

// --- Вспомогательная функция SHA256 ---

static std::vector<unsigned char> sha256(const std::string& input) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_HASH_HANDLE hHash = nullptr;

    DWORD hashObjectSize = 0;
    DWORD data = 0;
    DWORD hashSize = 0;

    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0)
        throw std::runtime_error("Failed to open SHA256 provider");

    BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH,
                      (PUCHAR)&hashObjectSize, sizeof(DWORD), &data, 0);

    BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH,
                      (PUCHAR)&hashSize, sizeof(DWORD), &data, 0);

    std::vector<unsigned char> hashObject(hashObjectSize);
    std::vector<unsigned char> hash(hashSize);

    if (BCryptCreateHash(hAlg, &hHash,
                         hashObject.data(), hashObjectSize,
                         nullptr, 0, 0) != 0)
        throw std::runtime_error("Failed to create hash");

    BCryptHashData(hHash,
                   (PUCHAR)input.data(),
                   (ULONG)input.size(),
                   0);

    BCryptFinishHash(hHash,
                     hash.data(),
                     hashSize,
                     0);

    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    return hash;
}

// --- AES-256 CBC ---

std::vector<unsigned char> CryptoService::encrypt(
    const std::vector<unsigned char>& data,
    const std::string& password)
{
    auto key = sha256(password);

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_KEY_HANDLE hKey = nullptr;

    DWORD dataLen = 0;
    DWORD blockLen = 0;
    DWORD cbData = 0;

    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, nullptr, 0) != 0)
        throw std::runtime_error("Failed to open AES provider");

    BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE,
                      (PUCHAR)BCRYPT_CHAIN_MODE_CBC,
                      sizeof(BCRYPT_CHAIN_MODE_CBC), 0);

    BCryptGetProperty(hAlg, BCRYPT_BLOCK_LENGTH,
                      (PUCHAR)&blockLen, sizeof(DWORD),
                      &cbData, 0);

    std::vector<unsigned char> iv(blockLen);
    BCryptGenRandom(nullptr, iv.data(), blockLen, BCRYPT_USE_SYSTEM_PREFERRED_RNG);

    if (BCryptGenerateSymmetricKey(hAlg, &hKey,
                                   nullptr, 0,
                                   key.data(), key.size(), 0) != 0)
        throw std::runtime_error("Failed to generate AES key");

    BCryptEncrypt(hKey,
                  (PUCHAR)data.data(),
                  (ULONG)data.size(),
                  nullptr,
                  iv.data(),
                  blockLen,
                  nullptr,
                  0,
                  &dataLen,
                  BCRYPT_BLOCK_PADDING);

    std::vector<unsigned char> ivCopy = iv;
    std::vector<unsigned char> encrypted(dataLen);

    BCryptEncrypt(hKey,
              (PUCHAR)data.data(),
              (ULONG)data.size(),
              nullptr,
              ivCopy.data(),
              blockLen,
              encrypted.data(),
              dataLen,
              &dataLen,
              BCRYPT_BLOCK_PADDING);


    encrypted.resize(dataLen);

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    // Вставляем IV в начало файла
    encrypted.insert(encrypted.begin(), iv.begin(), iv.end());

    return encrypted;
}

std::vector<unsigned char> CryptoService::decrypt(
    const std::vector<unsigned char>& data,
    const std::string& password)
{
    auto key = sha256(password);

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_KEY_HANDLE hKey = nullptr;

    DWORD blockLen = 0;
    DWORD cbData = 0;
    DWORD dataLen = 0;

    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, nullptr, 0) != 0)
        throw std::runtime_error("Failed to open AES provider");

    BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE,
                      (PUCHAR)BCRYPT_CHAIN_MODE_CBC,
                      sizeof(BCRYPT_CHAIN_MODE_CBC), 0);

    BCryptGetProperty(hAlg, BCRYPT_BLOCK_LENGTH,
                      (PUCHAR)&blockLen, sizeof(DWORD),
                      &cbData, 0);

    std::vector<unsigned char> iv(data.begin(), data.begin() + blockLen);
    std::vector<unsigned char> encrypted(data.begin() + blockLen, data.end());

    if (BCryptGenerateSymmetricKey(hAlg, &hKey,
                                nullptr, 0,
                                key.data(), key.size(), 0) != 0)
        throw std::runtime_error("Failed to generate AES key");

    std::vector<unsigned char> ivCopy = iv;

    // Первый вызов - только для вычисления размера
    BCryptDecrypt(hKey,
                encrypted.data(),
                encrypted.size(),
                nullptr,
                ivCopy.data(),
                blockLen,
                nullptr,
                0,
                &dataLen,
                BCRYPT_BLOCK_PADDING);

    // Нужно восстановить IV перед вторым вызовом
    ivCopy = iv;

    std::vector<unsigned char> decrypted(dataLen);

    if (BCryptDecrypt(hKey,
                    encrypted.data(),
                    encrypted.size(),
                    nullptr,
                    ivCopy.data(),
                    blockLen,
                    decrypted.data(),
                    dataLen,
                    &dataLen,
                    BCRYPT_BLOCK_PADDING) != 0)
        throw std::runtime_error("Decryption failed (wrong password?)");


    decrypted.resize(dataLen);

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    return decrypted;
}
