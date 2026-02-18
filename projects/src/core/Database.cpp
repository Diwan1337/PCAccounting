#include "Database.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include "../utils/DateUtils.h"

// Добавление

int Database::addEmployee(Employee employee) {
    employee.id = nextEmployeeId++;
    employees.push_back(employee);
    return employee.id;
}

int Database::addComputer(Computer computer) {
    if (!isInventoryNumberUnique(computer.inventoryNumber))
        throw std::runtime_error("Инвентарный номер уже существует: " + computer.inventoryNumber);
    if (!isSerialNumberUnique(computer.serialNumber))
        throw std::runtime_error("Серийный номер уже существует: " + computer.serialNumber);
    computer.id = nextComputerId++;
    computers.push_back(computer);
    return computer.id;
}

int Database::addEmployeeWithId(const Employee& employee) {
    if (employee.id <= 0)
        throw std::runtime_error("Некорректный ID сотрудника при загрузке");

    for (const auto& e : employees) {
        if (e.id == employee.id)
            throw std::runtime_error("Дублируется ID сотрудника при загрузке: " + std::to_string(employee.id));
    }

    employees.push_back(employee);

    if (employee.id >= nextEmployeeId)
        nextEmployeeId = employee.id + 1;

    return employee.id;
}

int Database::addComputerWithId(const Computer& computer) {
    if (computer.id <= 0)
        throw std::runtime_error("Некорректный ID компьютера при загрузке");

    for (const auto& c : computers) {
        if (c.id == computer.id)
            throw std::runtime_error("Дублируется ID компьютера при загрузке: " + std::to_string(computer.id));
    }

    computers.push_back(computer);

    if (computer.id >= nextComputerId)
        nextComputerId = computer.id + 1;

    return computer.id;
}

// Удаление

void Database::removeEmployee(int id) {
    employees.erase(
        std::remove_if(employees.begin(), employees.end(),
            [id](const Employee& e) { return e.id == id; }),
        employees.end()
    );
}

void Database::removeComputer(int id) {

    // Снять назначение у сотрудников
    for (auto& e : employees) {
        if (e.computerId.has_value() && e.computerId.value() == id) {
            e.computerId.reset();
        }
    }

    computers.erase(
        std::remove_if(computers.begin(), computers.end(),
            [id](const Computer& c) { return c.id == id; }),
        computers.end()
    );
}

// Обновление

bool Database::updateEmployee(const Employee& employee) {
    for (auto& e : employees) {
        if (e.id == employee.id) {
            e = employee;
            return true;
        }
    }
    return false;
}

bool Database::updateComputer(const Computer& computer) {
    for (const auto& c : computers) {
        if (c.id == computer.id)
            continue;
        if (c.inventoryNumber == computer.inventoryNumber)
            throw std::runtime_error("Инвентарный номер уже существует: " + computer.inventoryNumber);
        if (c.serialNumber == computer.serialNumber)
            throw std::runtime_error("Серийный номер уже существует: " + computer.serialNumber);
    }

    for (auto& c : computers) {
        if (c.id == computer.id) {
            c = computer;
            return true;
        }
    }
    return false;
}

// Поиск

Employee* Database::findEmployeeById(int id) {
    for (auto& e : employees)
        if (e.id == id)
            return &e;
    return nullptr;
}

Computer* Database::findComputerById(int id) {
    for (auto& c : computers)
        if (c.id == id)
            return &c;
    return nullptr;
}

// Проверка уникальности

bool Database::isInventoryNumberUnique(const std::string& inventoryNumber) const {
    for (const auto& c : computers)
        if (c.inventoryNumber == inventoryNumber)
            return false;
    return true;
}

bool Database::isSerialNumberUnique(const std::string& serialNumber) const {
    for (const auto& c : computers)
        if (c.serialNumber == serialNumber)
            return false;
    return true;
}

// Назначение ПК

bool Database::assignComputer(int employeeId, int computerId) {

    Employee* employee = findEmployeeById(employeeId);
    Computer* computer = findComputerById(computerId);

    if (!employee || !computer)
        return false;

    if (employee->status == "Уволен")
        return false;

    // Проверяем, не занят ли компьютер
    for (const auto& e : employees) {
        if (e.computerId.has_value() &&
            e.computerId.value() == computerId)
            return false;
    }

    employee->computerId = computerId;
    return true;
}

bool Database::unassignComputer(int employeeId) {
    Employee* employee = findEmployeeById(employeeId);
    if (!employee)
        return false;

    employee->computerId.reset();
    return true;
}

bool Database::unassignComputerByComputerId(int computerId) {
    for (auto& e : employees) {
        if (e.computerId.has_value() &&
            e.computerId.value() == computerId) {
            e.computerId.reset();
            return true;
        }
    }
    return false;
}

// Свободные ПК

std::vector<Computer> Database::getFreeComputers() const {

    std::vector<Computer> freeComputers;

    for (const auto& computer : computers) {

        bool assigned = false;

        for (const auto& employee : employees) {
            if (employee.computerId.has_value() &&
                employee.computerId.value() == computer.id) {
                assigned = true;
                break;
            }
        }

        if (!assigned)
            freeComputers.push_back(computer);
    }

    return freeComputers;
}

// Геттеры

const std::vector<Employee>& Database::getEmployees() const {
    return employees;
}

const std::vector<Computer>& Database::getComputers() const {
    return computers;
}

// Отчет

std::vector<Computer> Database::getComputersWithRamLessThan(int value) const {

    std::vector<Computer> result;

    for (const auto& c : computers) {
        if (c.ramSize < value)
            result.push_back(c);
    }

    return result;
}

// Поиск

std::vector<Employee> Database::findEmployeesByLastName(const std::string& name) const {

    std::vector<Employee> result;

    for (const auto& e : employees) {
        if (e.lastName.find(name) != std::string::npos)
            result.push_back(e);
    }

    return result;
}

std::vector<Computer> Database::findComputersByInventory(const std::string& inventory) const {

    std::vector<Computer> result;

    for (const auto& c : computers) {
        if (c.inventoryNumber.find(inventory) != std::string::npos)
            result.push_back(c);
    }

    return result;
}

// Валидация

void Database::validate() const {

    std::vector<std::string> errors;

    std::unordered_set<int> employeeIds;
    std::unordered_set<int> computerIds;
    std::unordered_set<int> assignedComputers;

    for (const auto& e : employees) {
        if (e.id <= 0)
            errors.push_back("Некорректный ID сотрудника: " + std::to_string(e.id));

        if (!employeeIds.insert(e.id).second)
            errors.push_back("Дублируется ID сотрудника: " + std::to_string(e.id));

        date_utils::validateDateField(e.employmentDate,
                                      "employmentDate (ID сотрудника " + std::to_string(e.id) + ")",
                                      errors,
                                      false);
    }

    std::unordered_set<std::string> inventoryNumbers;
    std::unordered_set<std::string> serialNumbers;

    for (const auto& c : computers) {
        if (c.id <= 0)
            errors.push_back("Некорректный ID компьютера: " + std::to_string(c.id));

        if (!computerIds.insert(c.id).second)
            errors.push_back("Дублируется ID компьютера: " + std::to_string(c.id));

        if (!inventoryNumbers.insert(c.inventoryNumber).second)
            errors.push_back("Дублируется инвентарный номер: " + c.inventoryNumber);

        if (!serialNumbers.insert(c.serialNumber).second)
            errors.push_back("Дублируется серийный номер: " + c.serialNumber);

        if (c.inventoryNumber.empty())
            errors.push_back("Инвентарный номер не может быть пустым (ID компьютера " + std::to_string(c.id) + ")");

        if (c.serialNumber.empty())
            errors.push_back("Серийный номер не может быть пустым (ID компьютера " + std::to_string(c.id) + ")");

        if (c.ramSize <= 0)
            errors.push_back("ramSize должен быть больше 0 (ID компьютера " + std::to_string(c.id) + ")");

        if (c.storageSize <= 0)
            errors.push_back("storageSize должен быть больше 0 (ID компьютера " + std::to_string(c.id) + ")");

        date_utils::validateDateField(c.commissioningDate,
                                      "commissioningDate (ID компьютера " + std::to_string(c.id) + ")",
                                      errors,
                                      false);
        date_utils::validateDateField(c.lastMaintenanceDate,
                                      "lastMaintenanceDate (ID компьютера " + std::to_string(c.id) + ")",
                                      errors,
                                      false);
        date_utils::validateDateField(c.warrantyExpirationDate,
                                      "warrantyExpirationDate (ID компьютера " + std::to_string(c.id) + ")",
                                      errors,
                                      true);
    }

    for (const auto& e : employees) {
        if (e.computerId.has_value()) {
            int compId = e.computerId.value();
            if (computerIds.find(compId) == computerIds.end()) {
                errors.push_back("Назначен несуществующий компьютер (ID компьютера " +
                                 std::to_string(compId) +
                                 ", ID сотрудника " + std::to_string(e.id) + ")");
            } else if (!assignedComputers.insert(compId).second) {
                errors.push_back("Компьютер назначен нескольким сотрудникам (ID компьютера " +
                                 std::to_string(compId) + ")");
            }
        }
    }

    if (!errors.empty()) {
        std::ostringstream message;
        message << "Ошибка валидации базы данных:\n";
        for (const auto& err : errors)
            message << "- " << err << "\n";
        throw std::runtime_error(message.str());
    }
}
