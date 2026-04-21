#pragma once
#include <vector>
#include "../core/Database.h"

class Serializer {
public:
    static std::vector<unsigned char> serialize(const Database& db);
    static Database deserialize(const std::vector<unsigned char>& data);
};
