#pragma once
#include <vector>
#include <optional>
#include <string>
#include "../models/Employee.h"
#include "../models/Computer.h"

class Database {
private:
    std::vector<Employee> employees;
    std::vector<Computer> computers;

    int nextEmployeeId = 1;
    int nextComputerId = 1;

public:
    // Добавление (ID генерируется внутри)
    int addEmployee(Employee employee);
    int addComputer(Computer computer);

    // Добавление с заданным ID (для загрузки)
    int addEmployeeWithId(const Employee& employee);
    int addComputerWithId(const Computer& computer);

    // Удаление
    void removeEmployee(int id);
    void removeComputer(int id);

    // Обновление
    bool updateEmployee(const Employee& employee);
    bool updateComputer(const Computer& computer);

    // Поиск
    Employee* findEmployeeById(int id);
    Computer* findComputerById(int id);

    // Проверка уникальности
    bool isInventoryNumberUnique(const std::string& inventoryNumber) const;
    bool isSerialNumberUnique(const std::string& serialNumber) const;

    // Назначение
    bool assignComputer(int employeeId, int computerId);
    bool unassignComputer(int employeeId);
    bool unassignComputerByComputerId(int computerId);

    // Свободные ПК
    std::vector<Computer> getFreeComputers() const;

    // Доступ к данным
    const std::vector<Employee>& getEmployees() const;
    const std::vector<Computer>& getComputers() const;

    // Отчет
    std::vector<Computer> getComputersWithRamLessThan(int value) const;

    // Поиск
    std::vector<Employee> findEmployeesByLastName(const std::string& name) const;
    std::vector<Computer> findComputersByInventory(const std::string& inventory) const;

    // Валидация
    void validate() const;
};
