#include "ApplicationController.h"
#include <stdexcept>

void ApplicationController::createNewDatabase(const std::string& password) {
    database = Database();
    currentPassword = password;
    loaded = true;
    dirty = true;
}

void ApplicationController::loadDatabase(const std::string& path,
                                         const std::string& password) {
    database = storage.loadDatabase(path, password);
    currentPassword = password;
    loaded = true;
    dirty = false;
}

void ApplicationController::saveDatabase(const std::string& path) {
    if (!loaded)
        throw std::runtime_error("База не загружена");

    database.validate();
    storage.saveDatabase(database, path, currentPassword);
    dirty = false;
}

int ApplicationController::addEmployee(const Employee& e) {
    int id = database.addEmployee(e);
    dirty = true;
    return id;
}

int ApplicationController::addComputer(const Computer& c) {
    int id = database.addComputer(c);
    dirty = true;
    return id;
}

bool ApplicationController::assignComputer(int empId, int compId) {
    bool result = database.assignComputer(empId, compId);
    if (result)
        dirty = true;
    return result;
}

void ApplicationController::removeEmployee(int id) {
    database.removeEmployee(id);
    dirty = true;
}

void ApplicationController::removeComputer(int id) {
    database.removeComputer(id);
    dirty = true;
}

bool ApplicationController::updateEmployee(const Employee& e) {
    bool result = database.updateEmployee(e);
    if (result)
        dirty = true;
    return result;
}

bool ApplicationController::updateComputer(const Computer& c) {
    bool result = database.updateComputer(c);
    if (result)
        dirty = true;
    return result;
}

const std::vector<Employee>& ApplicationController::getEmployees() const {
    return database.getEmployees();
}

const std::vector<Computer>& ApplicationController::getComputers() const {
    return database.getComputers();
}

std::vector<Computer> ApplicationController::getReportRamLessThan(int value) const {
    return database.getComputersWithRamLessThan(value);
}

std::vector<Computer> ApplicationController::getFreeComputers() const {
    return database.getFreeComputers();
}

bool ApplicationController::isInventoryNumberUnique(const std::string& inventoryNumber) const {
    return database.isInventoryNumberUnique(inventoryNumber);
}

bool ApplicationController::isSerialNumberUnique(const std::string& serialNumber) const {
    return database.isSerialNumberUnique(serialNumber);
}

bool ApplicationController::unassignComputerByComputerId(int computerId) {
    bool result = database.unassignComputerByComputerId(computerId);
    if (result)
        dirty = true;
    return result;
}

bool ApplicationController::isLoaded() const {
    return loaded;
}

bool ApplicationController::isDirty() const {
    return dirty;
}
