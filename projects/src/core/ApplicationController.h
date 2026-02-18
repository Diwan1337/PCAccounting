#pragma once

#include <string>
#include <vector>

#include "Database.h"
#include "../models/Employee.h"
#include "../models/Computer.h"
#include "../storage/StorageService.h"

class ApplicationController
{
private:
    Database database;
    StorageService storage;
    std::string currentPassword;
    bool loaded = false;
    bool dirty = false;

public:
    void createNewDatabase(const std::string& password);
    void loadDatabase(const std::string& path,
                      const std::string& password);
    void saveDatabase(const std::string& path);

    int addEmployee(const Employee& e);
    int addComputer(const Computer& c);

    bool assignComputer(int empId, int compId);

    void removeEmployee(int id);
    void removeComputer(int id);
    bool updateEmployee(const Employee& e);
    bool updateComputer(const Computer& c);

    const std::vector<Employee>& getEmployees() const;
    const std::vector<Computer>& getComputers() const;

    std::vector<Computer> getReportRamLessThan(int value) const;
    std::vector<Computer> getFreeComputers() const;
    bool isInventoryNumberUnique(const std::string& inventoryNumber) const;
    bool isSerialNumberUnique(const std::string& serialNumber) const;
    bool unassignComputerByComputerId(int computerId);

    bool isLoaded() const;
    bool isDirty() const;
};
