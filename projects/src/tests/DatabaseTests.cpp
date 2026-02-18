#include <iostream>
#include <cassert>
#include "../core/Database.h"

void runDatabaseTests() {

    std::cout << "Запуск тестов Database...\n";

    Database db;

    // 1. Добавление сотрудника
    Employee e1;
    e1.institute = "МИРЭА";
    e1.department = "ИБ";
    e1.lastName = "Иванов";
    e1.initials = "И.И";
    e1.position = "Преподаватель";

    int empId = db.addEmployee(e1);
    assert(empId == 1);
    assert(db.getEmployees().size() == 1);

    // 2. Добавление компьютера
    Computer c1;
    c1.inventoryNumber = "INV-1";
    c1.serialNumber = "SN-1";
    c1.manufacturer = "Dell";
    c1.model = "OptiPlex";
    c1.cpuModel = "i5";
    c1.ramSize = 16;

    int compId = db.addComputer(c1);
    assert(compId == 1);
    assert(db.getComputers().size() == 1);

    // 3. Назначение ПК
    bool assigned = db.assignComputer(empId, compId);
    assert(assigned == true);

    // 4. Нельзя назначить занятый ПК
    Employee e2;
    e2.institute = "МИРЭА";
    e2.department = "ИБ";
    e2.lastName = "Петров";
    e2.initials = "П.П";
    e2.position = "Ассистент";

    int emp2Id = db.addEmployee(e2);
    bool secondAssign = db.assignComputer(emp2Id, compId);
    assert(secondAssign == false);

    // 5. Поиск по фамилии
    auto found = db.findEmployeesByLastName("Иванов");
    assert(found.size() == 1);

    // 6. Фильтр RAM
    auto ramFiltered = db.getComputersWithRamLessThan(32);
    assert(ramFiltered.size() == 1);

    auto ramFiltered2 = db.getComputersWithRamLessThan(8);
    assert(ramFiltered2.size() == 0);

    // 7. Свободные ПК
    auto freePCs = db.getFreeComputers();
    assert(freePCs.size() == 0);

    // 8. Удаление сотрудника
    db.removeEmployee(empId);
    assert(db.getEmployees().size() == 1);

    // После удаления сотрудника ПК должен стать свободным
    auto freeAfterDelete = db.getFreeComputers();
    assert(freeAfterDelete.size() == 1);

    // 9. Удаление компьютера
    db.removeComputer(compId);
    assert(db.getComputers().size() == 0);

    std::cout << "Все тесты пройдены успешно.\n";
}
