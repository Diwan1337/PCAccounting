#include "StorageService.h"
#include "Serializer.h"
#include "../crypto/CryptoService.h"

#include <fstream>
#include <vector>
#include <stdexcept>

void StorageService::saveDatabase(const Database& db,
                                  const std::string& filePath,
                                  const std::string& password)
{
    db.validate();

    std::vector<unsigned char> rawData = Serializer::serialize(db);

    std::vector<unsigned char> encrypted =
        CryptoService::encrypt(rawData, password);

    std::ofstream out(filePath, std::ios::binary);
    if (!out.is_open())
        throw std::runtime_error("Cannot open file for writing");

    out.write(reinterpret_cast<const char*>(encrypted.data()),
              encrypted.size());
}

Database StorageService::loadDatabase(const std::string& filePath,
                                      const std::string& password)
{
    std::ifstream in(filePath, std::ios::binary);
    if (!in.is_open())
        throw std::runtime_error("Cannot open file for reading");

    std::vector<unsigned char> encrypted(
        (std::istreambuf_iterator<char>(in)),
        std::istreambuf_iterator<char>()
    );

    std::vector<unsigned char> decrypted =
        CryptoService::decrypt(encrypted, password);

    return Serializer::deserialize(decrypted);
}
