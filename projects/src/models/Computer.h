#pragma once
#include <string>

struct Computer {
    int id;
    std::string inventoryNumber;
    std::string serialNumber;
    std::string manufacturer;
    std::string model;
    std::string cpuModel;
    std::string chipset;

    int ramSize;
    std::string storageType;
    int storageSize;

    std::string roomNumber;
    std::string condition; // Рабочее / Требует ремонта / Списан

    std::string commissioningDate;
    std::string lastMaintenanceDate;
    std::string warrantyExpirationDate;
};
