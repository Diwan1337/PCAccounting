#pragma once
#include <string>
#include "../core/Database.h"

class StorageService {
public:
    void saveDatabase(const Database& db,
                      const std::string& filePath,
                      const std::string& password);

    Database loadDatabase(const std::string& filePath,
                          const std::string& password);
};
