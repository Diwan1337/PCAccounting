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
    // 0. Валидация
    db.validate();

    // 1. Сериализация
    std::vector<unsigned char> rawData = Serializer::serialize(db);

    // 2. Шифрование
    std::vector<unsigned char> encrypted =
        CryptoService::encrypt(rawData, password);

    // 3. Запись файла
    std::ofstream out(filePath, std::ios::binary);
    if (!out.is_open())
        throw std::runtime_error("Cannot open file for writing");

    out.write(reinterpret_cast<const char*>(encrypted.data()),
              encrypted.size());
}

Database StorageService::loadDatabase(const std::string& filePath,
                                      const std::string& password)
{
    // 1. Читаем файл
    std::ifstream in(filePath, std::ios::binary);
    if (!in.is_open())
        throw std::runtime_error("Cannot open file for reading");

    std::vector<unsigned char> encrypted(
        (std::istreambuf_iterator<char>(in)),
        std::istreambuf_iterator<char>()
    );

    // 2. Дешифрование
    std::vector<unsigned char> decrypted =
        CryptoService::decrypt(encrypted, password);

    // 3. Десериализация
    return Serializer::deserialize(decrypted);
}
