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
    int addEmployee(Employee employee);
    int addComputer(Computer computer);

    int addEmployeeWithId(const Employee& employee);
    int addComputerWithId(const Computer& computer);

    void removeEmployee(int id);
    void removeComputer(int id);

    bool updateEmployee(const Employee& employee);
    bool updateComputer(const Computer& computer);

    Employee* findEmployeeById(int id);
    Computer* findComputerById(int id);

    bool isInventoryNumberUnique(const std::string& inventoryNumber) const;
    bool isSerialNumberUnique(const std::string& serialNumber) const;

    bool assignComputer(int employeeId, int computerId);
    bool unassignComputer(int employeeId);
    bool unassignComputerByComputerId(int computerId);

    std::vector<Computer> getFreeComputers() const;

    const std::vector<Employee>& getEmployees() const;
    const std::vector<Computer>& getComputers() const;

    std::vector<Computer> getComputersWithRamLessThan(int value) const;

    std::vector<Employee> findEmployeesByLastName(const std::string& name) const;
    std::vector<Computer> findComputersByInventory(const std::string& inventory) const;

    void validate() const;
};
