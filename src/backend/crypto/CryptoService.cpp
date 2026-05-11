#include "CryptoService.h"

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#include <stdexcept>
#include <vector>

static std::vector<unsigned char> sha256(const std::string& input) {
    std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
    if (SHA256(reinterpret_cast<const unsigned char*>(input.data()),
               input.size(),
               hash.data()) == nullptr) {
        throw std::runtime_error("Failed to calculate SHA-256");
    }
    return hash;
}

static std::runtime_error makeOpenSslError(const std::string& prefix) {
    unsigned long errorCode = ERR_get_error();
    char buffer[256] = {};
    if (errorCode != 0)
        ERR_error_string_n(errorCode, buffer, sizeof(buffer));

    std::string message = prefix;
    if (errorCode != 0) {
        message += ": ";
        message += buffer;
    }
    return std::runtime_error(message);
}

std::vector<unsigned char> CryptoService::encrypt(
    const std::vector<unsigned char>& data,
    const std::string& password)
{
    auto key = sha256(password);
    const EVP_CIPHER* cipher = EVP_aes_256_cbc();
    const int ivLength = EVP_CIPHER_iv_length(cipher);
    const int blockSize = EVP_CIPHER_block_size(cipher);

    std::vector<unsigned char> iv(ivLength);
    if (RAND_bytes(iv.data(), ivLength) != 1)
        throw makeOpenSslError("Failed to generate IV");

    EVP_CIPHER_CTX* rawContext = EVP_CIPHER_CTX_new();
    if (rawContext == nullptr)
        throw makeOpenSslError("Failed to create OpenSSL cipher context");

    struct CipherContextGuard {
        EVP_CIPHER_CTX* context;
        ~CipherContextGuard() {
            EVP_CIPHER_CTX_free(context);
        }
    } contextGuard{rawContext};

    if (EVP_EncryptInit_ex(rawContext, cipher, nullptr, key.data(), iv.data()) != 1)
        throw makeOpenSslError("Failed to initialize AES-256-CBC encryption");

    std::vector<unsigned char> encrypted(data.size() + blockSize);
    int written = 0;
    int finalWritten = 0;

    if (EVP_EncryptUpdate(rawContext,
                          encrypted.data(),
                          &written,
                          data.data(),
                          static_cast<int>(data.size())) != 1) {
        throw makeOpenSslError("Failed to encrypt data");
    }

    if (EVP_EncryptFinal_ex(rawContext,
                            encrypted.data() + written,
                            &finalWritten) != 1) {
        throw makeOpenSslError("Failed to finalize encryption");
    }

    encrypted.resize(written + finalWritten);
    encrypted.insert(encrypted.begin(), iv.begin(), iv.end());
    return encrypted;
}

std::vector<unsigned char> CryptoService::decrypt(
    const std::vector<unsigned char>& data,
    const std::string& password)
{
    auto key = sha256(password);
    const EVP_CIPHER* cipher = EVP_aes_256_cbc();
    const int ivLength = EVP_CIPHER_iv_length(cipher);
    const int blockSize = EVP_CIPHER_block_size(cipher);

    if (data.size() < static_cast<size_t>(ivLength))
        throw std::runtime_error("Encrypted payload is too short");

    std::vector<unsigned char> iv(data.begin(), data.begin() + ivLength);
    std::vector<unsigned char> encrypted(data.begin() + ivLength, data.end());

    EVP_CIPHER_CTX* rawContext = EVP_CIPHER_CTX_new();
    if (rawContext == nullptr)
        throw makeOpenSslError("Failed to create OpenSSL cipher context");

    struct CipherContextGuard {
        EVP_CIPHER_CTX* context;
        ~CipherContextGuard() {
            EVP_CIPHER_CTX_free(context);
        }
    } contextGuard{rawContext};

    if (EVP_DecryptInit_ex(rawContext, cipher, nullptr, key.data(), iv.data()) != 1)
        throw makeOpenSslError("Failed to initialize AES-256-CBC decryption");

    std::vector<unsigned char> decrypted(encrypted.size() + blockSize);
    int written = 0;
    int finalWritten = 0;

    if (EVP_DecryptUpdate(rawContext,
                          decrypted.data(),
                          &written,
                          encrypted.data(),
                          static_cast<int>(encrypted.size())) != 1) {
        throw makeOpenSslError("Failed to decrypt data");
    }

    if (EVP_DecryptFinal_ex(rawContext,
                            decrypted.data() + written,
                            &finalWritten) != 1) {
        throw std::runtime_error("Decryption failed (wrong password or corrupted data)");
    }

    decrypted.resize(written + finalWritten);
    return decrypted;
}
