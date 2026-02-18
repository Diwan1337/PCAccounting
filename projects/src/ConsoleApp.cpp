#include <iostream>
#include <string>
#include <limits>
#include <filesystem>
#define NOMINMAX
#include <windows.h>

#include "core/Database.h"
#include "storage/StorageService.h"

void printMenu() {
    std::cout << "\n===== Система учета ПК =====\n";
    std::cout << "1. Создать новую БД\n";
    std::cout << "2. Загрузить БД\n";
    std::cout << "3. Добавить сотрудника\n";
    std::cout << "4. Добавить компьютер\n";
    std::cout << "5. Назначить ПК сотруднику\n";
    std::cout << "6. Удалить сотрудника\n";
    std::cout << "7. Удалить компьютер\n";
    std::cout << "8. Сохранить БД\n";
    std::cout << "9. Показать статистику\n";
    std::cout << "10. Отчет: ПК с RAM меньше заданного\n";
    std::cout << "11. Показать всех сотрудников\n";
    std::cout << "12. Показать все компьютеры\n";
    std::cout << "13. Показать закрепленные ПК\n";
    std::cout << "14. Поиск сотрудника по фамилии\n";
    std::cout << "15. Поиск ПК по инвентарному номеру\n";
    std::cout << "16. Редактировать сотрудника\n";
    std::cout << "17. Редактировать компьютер\n";
    std::cout << "0. Выход\n";
}

bool inputInt(const std::string& prompt, int& result) {
    while (true) {
        std::string line;
        std::cout << prompt;
        std::getline(std::cin, line);

        if (line.empty())
            return false;

        try {
            result = std::stoi(line);
            return true;
        }
        catch (...) {
            std::cout << "Некорректное число\n";
        }
    }
}

std::string inputLine(const std::string& prompt, bool allowEmpty = false) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);

    if (allowEmpty)
        return value;

    while (value.empty()) {
        std::cout << "Поле не может быть пустым. Повторите ввод: ";
        std::getline(std::cin, value);
    }

    return value;
}

int runConsole() {

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Database db;
    StorageService storage;

    std::string password;
    bool dbLoaded = false;

    while (true) {

        printMenu();

        int choice;
        if (!inputInt("Выберите пункт: ", choice))
            continue;

        try {

            // 1. Создать новую БД
            if (choice == 1) {

                db = Database();
                password = inputLine("Введите пароль для новой БД (пример: myStrongPass123): ");
                dbLoaded = true;

                std::cout << "Новая БД создана\n";
            }

            // 2. Загрузить БД
            else if (choice == 2) {

                std::string path = inputLine("Введите путь к файлу (пример: C:\\\\data\\\\office.db): ");
                password = inputLine("Введите пароль (пример: myStrongPass123): ");

                if (!std::filesystem::exists(path)) {
                    std::cout << "Файл не найден\n";
                    continue;
                }

                db = storage.loadDatabase(path, password);
                dbLoaded = true;

                std::cout << "БД успешно загружена\n";
            }

            // 3. Добавить сотрудника
            else if (choice == 3) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                Employee e;

                e.institute = inputLine("Институт (пример: ИТ): ");
                e.department = inputLine("Кафедра (пример: КТ): ");
                e.lastName = inputLine("Фамилия (пример: Иванов): ");
                e.initials = inputLine("Инициалы (пример: И.И.): ");
                e.position = inputLine("Должность (пример: инженер): ");

                e.phone = "";
                e.email = "";
                e.employmentDate = "";
                e.status = "Активен";
                e.computerId = std::nullopt;

                int newId = db.addEmployee(e);

                std::cout << "Сотрудник добавлен. ID: " << newId << "\n";
            }

            // 4. Добавить компьютер
            else if (choice == 4) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                Computer c;

                c.inventoryNumber = inputLine("Инвентарный номер (пример: PC-001): ");
                c.serialNumber = inputLine("Серийный номер (пример: SN123456): ");
                c.manufacturer = inputLine("Производитель (пример: Lenovo): ");
                c.model = inputLine("Модель (пример: ThinkPad T14): ");
                c.cpuModel = inputLine("CPU (пример: Intel i5-1145G7): ");

                if (!inputInt("RAM (ГБ, пример: 16): ", c.ramSize))
                    continue;

                if (c.ramSize <= 0) {
                    std::cout << "RAM должен быть больше 0\n";
                    continue;
                }

                c.storageType = "SSD";
                c.storageSize = 512;
                c.roomNumber = "";
                c.condition = "Рабочее";
                c.commissioningDate = "";
                c.lastMaintenanceDate = "";
                c.warrantyExpirationDate = "";

                int newId = db.addComputer(c);

                std::cout << "Компьютер добавлен. ID: " << newId << "\n";
            }

            // 5. Назначить ПК
            else if (choice == 5) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                int empId, compId;

                if (!inputInt("ID сотрудника (пример: 1): ", empId))
                    continue;

                if (!inputInt("ID компьютера (пример: 1): ", compId))
                    continue;

                Employee* emp = db.findEmployeeById(empId);
                Computer* comp = db.findComputerById(compId);

                if (!emp || !comp) {
                    std::cout << "Некорректные ID\n";
                    continue;
                }

                if (emp->status == "Уволен") {
                    std::cout << "Нельзя назначить ПК сотруднику со статусом \"Уволен\"\n";
                    continue;
                }

                if (db.assignComputer(empId, compId))
                    std::cout << "Назначение выполнено\n";
                else
                    std::cout << "Ошибка назначения\n";
            }

            // 6. Удалить сотрудника
            else if (choice == 6) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                int id;
                if (!inputInt("ID сотрудника (пример: 1): ", id))
                    continue;

                if (!db.findEmployeeById(id)) {
                    std::cout << "ID не найден\n";
                    continue;
                }

                std::string confirm = inputLine("Подтвердить удаление? (yes/no): ");

                if (confirm == "yes") {
                    db.removeEmployee(id);
                    std::cout << "Удалено\n";
                }
            }

            // 7. Удалить компьютер
            else if (choice == 7) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                int id;
                if (!inputInt("ID компьютера (пример: 1): ", id))
                    continue;

                if (!db.findComputerById(id)) {
                    std::cout << "ID не найден\n";
                    continue;
                }

                std::string confirm = inputLine("Подтвердить удаление? (yes/no): ");

                if (confirm == "yes") {
                    db.removeComputer(id);
                    std::cout << "Удалено\n";
                }
            }

            // 8. Сохранить БД
            else if (choice == 8) {

                if (!dbLoaded) {
                    std::cout << "Нет БД для сохранения\n";
                    continue;
                }

                std::string path = inputLine(
                    "Введите путь для сохранения и имя файла (пример: C:\\\<существующий каталог>\\test.db): ");

                std::filesystem::path fsPath(path);
                std::filesystem::path dir = fsPath.has_parent_path()
                                                ? fsPath.parent_path()
                                                : std::filesystem::current_path();

                if (!std::filesystem::exists(dir)) {
                    std::cout << "Каталог не существует\n";
                    continue;
                }

                storage.saveDatabase(db, path, password);

                std::cout << "Сохранено\n";
            }

            // 9. Статистика
            else if (choice == 9) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                std::cout << "Сотрудников: "
                          << db.getEmployees().size() << "\n";

                std::cout << "Компьютеров: "
                          << db.getComputers().size() << "\n";
            }

            // 10. Отчет по RAM
            else if (choice == 10) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                int value;
                if (!inputInt("Введите порог RAM (ГБ, пример: 8): ", value))
                    continue;

                auto result = db.getComputersWithRamLessThan(value);

                std::cout << "Найдено: " << result.size() << "\n";

                for (const auto& c : result) {
                    std::cout << "ID: " << c.id
                              << " | " << c.model
                              << " | RAM: " << c.ramSize << "\n";
                }
            }

            // 11. Показать сотрудников
            else if (choice == 11) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                for (const auto& e : db.getEmployees()) {
                    std::cout << "ID: " << e.id
                              << " | " << e.lastName
                              << " | " << e.position << "\n";
                }
            }

            // 12. Показать компьютеры
            else if (choice == 12) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                for (const auto& c : db.getComputers()) {
                    std::cout << "ID: " << c.id
                              << " | " << c.model
                              << " | RAM: " << c.ramSize << "\n";
                }
            }

            // 13. Показать закрепленные ПК
            else if (choice == 13) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                for (const auto& e : db.getEmployees()) {

                    if (e.computerId.has_value()) {

                        Computer* c =
                            db.findComputerById(e.computerId.value());

                        if (c) {
                            std::cout << e.lastName
                                      << " -> "
                                      << c->model << "\n";
                        }
                    }
                }
            }
            
            // 14. Поиск сотрудника по фамилии
            else if (choice == 14) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                std::string name =
                    inputLine("Введите фамилию для поиска (пример: Иванов): ");

                auto result = db.findEmployeesByLastName(name);

                std::cout << "Найдено: "
                          << result.size() << "\n";

                for (const auto& e : result) {
                    std::cout << "ID: " << e.id
                              << " | " << e.lastName
                              << " | " << e.position << "\n";
                }
            }

            // 15. Поиск ПК по инвентарному номеру
            else if (choice == 15) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                std::string inv =
                    inputLine("Введите инвентарный номер (пример: PC-001): ");

                auto result =
                    db.findComputersByInventory(inv);

                std::cout << "Найдено: "
                          << result.size() << "\n";

                for (const auto& c : result) {
                    std::cout << "ID: " << c.id
                              << " | " << c.model
                              << " | RAM: "
                              << c.ramSize << "\n";
                }
            }

            // 16. Редактировать сотрудника
            else if (choice == 16) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                int id;

                if (!inputInt("Введите ID сотрудника (пример: 1): ", id))
                    continue;

                Employee* e =
                    db.findEmployeeById(id);

                if (!e) {
                    std::cout << "Сотрудник не найден\n";
                    continue;
                }

                std::cout << "Текущая должность: "
                          << e->position << "\n";

                std::string newPos =
                    inputLine("Новая должность (пример: ведущий инженер): ");

                std::string newStatus =
                    inputLine("Новый статус (пример: Активен/Уволен): ");

                e->position = newPos;
                e->status = newStatus;

                std::cout << "Обновлено\n";
            }

            // 17. Редактировать компьютер
            else if (choice == 17) {

                if (!dbLoaded) {
                    std::cout << "Сначала создайте или загрузите БД\n";
                    continue;
                }

                int id;

                if (!inputInt("Введите ID компьютера (пример: 1): ", id))
                    continue;

                Computer* c =
                    db.findComputerById(id);

                if (!c) {
                    std::cout << "Компьютер не найден\n";
                    continue;
                }

                std::cout << "Текущая модель: "
                          << c->model << "\n";

                std::string newModel =
                    inputLine("Новая модель (пример: Dell Latitude 5440): ");

                int newRam;

                if (!inputInt("Новый объем RAM (ГБ, пример: 32): ",
                              newRam))
                    continue;

                if (newRam <= 0) {
                    std::cout << "RAM должен быть больше 0\n";
                    continue;
                }

                c->model = newModel;
                c->ramSize = newRam;

                std::cout << "Обновлено\n";
            }

            // 0. Выход
            else if (choice == 0) {
                std::cout << "Выход...\n";
                break;
            }

            else {
                std::cout << "Неизвестный пункт меню\n";
            }

        }
        catch (const std::exception& ex) {
            std::cout << "Ошибка: "
                      << ex.what() << "\n";
        }
    }

    return 0;
}
